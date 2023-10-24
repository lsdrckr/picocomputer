#include "ordonnanceur.h"

void task0();
void task1();
void task2();

struct task_t task[3] = {
    {task0, 0x0600, 1},
    {task1, 0x0700, 1},
    {task2, 0x0800, 1}
    
};
int currentTask;

void setHighOutput(volatile uint8_t *port, volatile uint8_t pin){
	*port |= (1<<pin);
}

void setLowOutput(volatile uint8_t *port, volatile uint8_t pin){
	*port &= ~(1<<pin);
}

void reverseOutput(volatile uint8_t *port, volatile uint8_t pin){
    *port ^= (1<<pin);
}

void task0(){
    while(1){
        _delay_ms(100);
        setHighOutput(&PORTD, PD7);
        _delay_ms(100);
        setLowOutput(&PORTD, PD7);
    }
}

void task1(){
    while(1){
        _delay_ms(100);
        setHighOutput(&PORTD, PD1);
        _delay_ms(100);
        setLowOutput(&PORTD, PD1);
    }
}

void task2(){
    while(1){
        _delay_ms(100);
        setHighOutput(&PORTD, PD4);
        _delay_ms(100);
        setLowOutput(&PORTD, PD4);
    }
}


void initMinuteur(int diviseur,long periode){
    TCCR1A=0;               // Le mode choisi n'utilise pas ce registre
    TCCR1B=(1<<CTC1);       // Réinitialisation du minuteur sur expiration
    switch(diviseur){
        case    8: TCCR1B |= (1<<CS11); break;
        case   64: TCCR1B |= (1<<CS11 | 11<<CS10); break;
        case  256: TCCR1B |= (1<<CS12); break;
        case 1024: TCCR1B |= (1<<CS12 | 1<<CS10); break;
    }
    // Un cycle prend 1/F_CPU secondes.
    // Un pas de compteur prend diviseur/F_CPU secondes.
    // Pour une periode en millisecondes, il faut (periode/1000)/(diviseur/F_CPU) pas
    // soit (periode*F_CPU)/(1000*diviseur)
    OCR1A=F_CPU/1000*periode/diviseur;  // Calcul du pas
    TCNT1=0;                // Compteur initialisé
    TIMSK1=(1<<OCIE1A);     // Comparaison du compteur avec OCR1A
}

void initTask(int taskId){
    int save = SP;
    SP = task[taskId].sp;
    uint16_t address = (uint16_t)task[taskId].addr;
    asm volatile("push %0" : : "r" (address & 0x00ff) );
    asm volatile("push %0" : : "r" ((address & 0xff00)>>8) );
    SAVE_REGISTER();
    task[taskId].sp = SP;
    SP = save;
}

void scheduler (){
    currentTask ++;
    if(currentTask == NB_TASK) currentTask = 0;
}

ISR(TIMER1_COMPA_vect,ISR_NAKED){
    // Sauvegarde du contexte de la tâche interrompue
    SAVE_REGISTER();
    task[currentTask].sp = SP;
    // Appel à l'ordonnanceur
    scheduler();
    // Récupération du contexte de la tâche ré-activée
    SP = task[currentTask].sp; 
    RESTORE_REGISTER();
    
    asm volatile("reti");
}

void setup(){
    cli();
    
    // currentTask à 0
    currentTask = 0;
    
    // Setup des tâches    
    for(int i = 0; i < NB_TASK; i++){
        initTask(i);
    }
    
    // Setup des leds
    DDRD |= (1<<PD7);
    DDRD |= (1<<PD1);
    DDRD |= (1<<PD4);
    setLowOutput(&PORTD, PD7);
    setLowOutput(&PORTD, PD1);
    setLowOutput(&PORTD, PD4);
    
    // Setup du minuteur 
    initMinuteur(1024,20);
    
    sei(); 
}

int main(){
    setup();
    SP = task[currentTask].sp;
    task0();
}

// D5 PB0 D4 PD4 D3 PD1

#include "ordonnanceur.h"

struct task_t{
    void (*start)(void);
    uint16_t *Spointer;
    int state;
};

struct task_t task[3];
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
    _delay_ms(100);
    setHighOutput(&PORTD, PD7);
    _delay_ms(100);
    setLowOutput(&PORTD, PD7);
}

void task1(){
    _delay_ms(100);
    setHighOutput(&PORTD, PD1);
    _delay_ms(100);
    setLowOutput(&PORTD, PD1);
}

void task2(){
    _delay_ms(100);
    setHighOutput(&PORTD, PD4);
    _delay_ms(100);
    setLowOutput(&PORTD, PD4);
}


void init_minuteur(int diviseur,long periode){
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

void scheduler (){
    currentTask ++;
    if(currentTask == NB_TASK) currentTask = 0;
}

ISR(TIMER1_COMPA_vect,ISR_NAKED){
    // Sauvegarde du contexte de la tâche interrompue
    SAVE_REGISTER();
    task[currentTask].Spointer = (uint16_t*) SP;
    
    // Appel à l'ordonnanceur
    scheduler();
    // Récupération du contexte de la tâche ré-activée
    SP = (uint16_t) task[currentTask].Spointer;
    RESTORE_REGISTER();
    
    asm volatile("reti");
}

void setup(){
    cli();
    
    task[0].start = task0;
    *(task[0].Spointer) = 0x8000;
    task[0].state = 1;
    
    task[1].start = task1;
    *(task[1].Spointer) = 0x6000;
    task[1].state = 1;
    
    task[2].start = task2;
    *(task[2].Spointer) = 0x4000;
    task[2].state = 1;
    
    // currentTask à 0
    currentTask = 0;
    // Setup des leds
    DDRD |= (1<<PD7);
    DDRD |= (1<<PD1);
    DDRD |= (1<<PD4);
    setLowOutput(&PORTD, PD7);
    setLowOutput(&PORTD, PD1);
    setLowOutput(&PORTD, PD4);
    
    // Setup du minuteur 
    init_minuteur(1024,20);
    
    sei();
}

int main(){
    setup();
    while(1){
    }
}

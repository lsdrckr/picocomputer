#include "ordonnanceur.h"

task_t task[3];
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

void wait(int taskId, int wait_ms){
    task[taskId].state = SLEEP;
    sleep_t wait;
    wait.reason = DELAY_SLEEPING;
    wait.data = wait_ms;
    task[taskId].sleep = wait;
}

void task0(){
    DDRD |= (1<<PD7);
    while(1){
        wait(0, 4000);
        reverseOutput(&PORTD, PD7);
    }
}

void task1(){
    DDRD |= (1<<PD1);
    while(1){
        _delay_ms(100);
        reverseOutput(&PORTD, PD1);
    }
}

void task2(){
    DDRD |= (1<<PD4);
    while(1){
        _delay_ms(100);
        reverseOutput(&PORTD, PD4);
    }
}


void initMinuteur(int diviseur,long periode){
    TCCR1A=0;                                     // Le mode choisi n'utilise pas ce registre
    TCCR1B=(1<<CTC1);                             // Réinitialisation du minuteur sur expiration
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
    OCR1A=F_CPU/1000*periode/diviseur;          // Calcul du pas
    TCNT1=0;                                    // Compteur initialisé
    TIMSK1=(1<<OCIE1A);                         // Comparaison du compteur avec OCR1A
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
    // for(int i=0; i < NB_TASK; i++){
    //     if(task[currentTask].state == SLEEP && task[currentTask].sleep.reason == DELAY_SLEEPING){
    // 
    //         if(task[currentTask].sleep.data <= 0){
    //             task[currentTask].state = AWAKE;
    //         }else{
    //         }
    //     } 
    // }
    
    do{
        currentTask ++;
        if(currentTask == NB_TASK) currentTask = 0;
    }while(task[currentTask].state == SLEEP);                // Attention si tous les processus sont à l'arrêt
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
    task[0].addr = task0;
    task[1].addr = task1;
    task[2].addr = task2;
    
    task[0].sp = 0x0600;
    task[1].sp = 0x0800;
    task[2].sp = 0x0200;

    for(int i = 0; i < NB_TASK; i++){
        initTask(i);
        task[i].state = AWAKE;
    }
 
    // Setup du minuteur 
    initMinuteur(1024,20);
    
    sei(); 
}

int main(){
    setup();
    SP = task[currentTask].sp;
    task0();
}

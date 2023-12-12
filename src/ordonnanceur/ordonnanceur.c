#include "ordonnanceur.h"

task_t task[NB_TASK];
uint8_t currentTask=0;
uint8_t keyAscii = 'a';
int counter;
he10_t connectorsList[MAX_DEVICES];

void scheduler (){
    for(int i=0; i<NB_TASK; i++){
        if(task[i].state == SLEEP && task[i].sleep.reason == DELAY_SLEEPING){
            
            // Récupérer la différence de temps
            uint16_t difftime_ms = 20;
            if(TCNT1 != 0){
                difftime_ms = TCNT1*200/OCR1A/10; // On multiplie par 10 pour ne pas avoir de problème avec les nombres flottants
                TCNT1 = 0;
            }
            task[i].sleep.data -= difftime_ms;
            
            if(task[i].sleep.data <= 0){
                task[i].state = AWAKE;
            }
        }
    }
    
    do{
        currentTask ++;
        if(currentTask >= NB_TASK) currentTask = 0; // Attention si tous les processus sont à l'arrêt
    }while(task[currentTask].state == SLEEP);
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

void selectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss){
    // Abaisser la ligne SS pour sélectionner le périphérique
    *ssPort &= ~(1 << ss);
}

void unselectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss){
    // Lever la ligne SS pour désélectionner le périphérique
    *ssPort |= (1 << ss);
}

uint8_t transferSPI(uint8_t data) {
    // Charger les données dans le registre de données
    SPDR = data;

    // Attendre que la transmission soit terminée
    while (!(SPSR & (1 << SPIF)));

    // Retourner les données reçues
    return SPDR;
}

void wait(uint8_t reason, uint16_t data){
    cli();
    task[currentTask].state = SLEEP;
    sleep_t sleep;
    sleep.reason = reason;
    sleep.data = data;
    task[currentTask].sleep = sleep;
    TCNT1 = 0;
    sei();
    TIMER1_COMPA_vect();
}

int indexOf(uint8_t device){
    for(int i = 0; i<MAX_DEVICES; i++){
        if (connectorsList[i].device == device){
            return i;
        }
    }
    return -1;
}

void initConnectorsList(){
    uint8_t data;
    connectorsList[0].port = &PORTC;
    connectorsList[0].pin = SS2;
    
    connectorsList[1].port = &PORTC;
    connectorsList[1].pin = SS3;
    
    connectorsList[2].port = &PORTD;
    connectorsList[2].pin = SS4;
    
    connectorsList[3].port = &PORTD;
    connectorsList[3].pin = SS5;
    
    connectorsList[4].port = &PORTD;
    connectorsList[4].pin = SS6;
    
    
    
    for(int i = 0; i<MAX_DEVICES; i++){
        selectSlaveSPI(connectorsList[i].port, connectorsList[i].pin);
        transferSPI(0x00);
        wait(DELAY_SLEEPING, 20);
        data = transferSPI(0x01);
        connectorsList[i].device = data;
        unselectSlaveSPI(connectorsList[i].port, connectorsList[i].pin);
    }
}

void initSPI(){
    // MOSI SCK SS2 SS3 SS4 SS5 SS6 en sortie

    DDRB |= (1<<MOSI);
    DDRB |= (1<<SCK);
    DDRB |= (1<<SS);

    DDRC |= (1<<SS2);
    DDRC |= (1<<SS3);
    DDRD |= (1<<SS4);
    DDRD |= (1<<SS5);
    DDRD |= (1<<SS6);

    // MISO en entrée

    DDRB &= ~(1<<MISO);

    // Sélection d'aucun escalave
    
    PORTB |= (1<<SS);
    PORTC |= (1<<SS2);
    PORTC |= (1<<SS3);
    PORTD |= (1<<SS4);
    PORTD |= (1<<SS5);
    PORTD |= (1<<SS6);

    // Configurer le registre SPCR (SPI Control Register)
    
    SPCR |= (1 << SPE) | (1 << MSTR);

    // Configurer la vitesse de transmission Diviseur 64 SPR1 1 SPR0 1 SPI2X 0
    
    SPCR |= (1 << SPR1); 
    SPCR |= (1 << SPR0);
    
    initConnectorsList();
}

uint8_t transferDataTo(uint8_t device, uint8_t data){
    int i = indexOf(device);
    selectSlaveSPI(connectorsList[i].port, connectorsList[i].pin);
    uint8_t answer = transferSPI(data);
    wait(DELAY_SLEEPING, 100);
    unselectSlaveSPI(connectorsList[i].port, connectorsList[i].pin);
    return answer;
}

uint8_t grabKey(){
    uint8_t key;
    transferDataTo(KEYBOARD,0x55);
    wait(DELAY_SLEEPING, 20);
    key = transferDataTo(KEYBOARD,0x55);
    return key;
}

uint8_t bufferSize(){
    uint8_t buffer_size;
    transferDataTo(KEYBOARD, 0x01);
    wait(DELAY_SLEEPING, 20);
    buffer_size = transferDataTo(KEYBOARD, 0x01);
    return buffer_size;
}

void grabKeys(uint8_t keyList[bufferSize()]){
    uint8_t buffer_size = bufferSize();
    
    for (int i = 0; i<buffer_size; i++){
        wait(DELAY_SLEEPING, 20);
        uint8_t buffer = transferDataTo(KEYBOARD, 0x00);
        keyList[i] = buffer;
    }
}

void task0(){ // processus défault ne dort jamais
    while(1){
        _delay_ms(100);
    }
}

void readSerial(){
    //Test Keyboard
    initSPI();
    
    DDRC |= (1<<PC3);

    while(1){
        transferDataTo(KEYBOARD, 0x01);
    }

    uint8_t key = grabKey();
    if (key = 'c') PORTC ^= PC3;
    
    // while(1){
    //     _delay_ms(100);
    // }

    // DEBUG keyboard
//     initSPI();
//     DDRD &= ~(1<<INT3);
//     
//     while(1){
//         selectSlaveSPI(&PORTD, SS4);
//         uint8_t response = transferSPI(0x01);
//         if(response == 0x01){
//             PORTD ^= (1<<SS5);
//         }
//         wait(DELAY_SLEEPING, 1000);
//         unselectSlaveSPI(&PORTD, SS4);
//         wait(DELAY_SLEEPING, 1000);
        // if(PIND & (1<<INT3)){
        //     PORTD ^= (1<<SS4);
        // }          
    // }
}

void writeSerial(){
        while(1){
        _delay_ms(100);
    }
}

void WriteMatrice(){ 
    while(1){
        _delay_ms(100);
    }
}

void Write7Segment(){
    while(1){
        _delay_ms(100);
    }

    // counter = 0;
    // initSPI();
    // while(1){
    //     //Calcul
    //     counter++;
    //     if (counter > 9999) counter = 0;
    //     uint8_t digit1 = counter/1000;
    //     uint8_t digit2 = (counter - digit1*1000) / 100;
    //     uint8_t digit3 = (counter - digit1*1000 - digit2*100) / 10;
    //     uint8_t digit4 = (counter - digit1*1000 - digit2*100 - digit3*10);

    //     selectSlaveSPI(&PORTC, SS3);
    //     transferSPI(0x76);
    //     transferSPI(digit1);
    //     transferSPI(digit2);
    //     transferSPI(digit3);
    //     transferSPI(digit4);
    //     wait(DELAY_SLEEPING, 100);
    //     unselectSlaveSPI(&PORTC, SS3);
    // }
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
    OCR1A= F_CPU/1000*periode/diviseur;          // Calcul du pas
    TCNT1=0;                                    // Compteur initialisé
    TIMSK1=(1<<OCIE1A);                         // Comparaison du compteur avec OCR1A
}

void initTask(uint8_t taskId){
    uint16_t save = SP;
    SP = task[taskId].sp;
    uint16_t address = (uint16_t)task[taskId].addr;
    asm volatile("push %0" : : "r" (address & 0x00ff) );
    asm volatile("push %0" : : "r" ((address & 0xff00)>>8) );
    SAVE_REGISTER();
    task[taskId].sp = SP;
    SP = save;
}

void setup(){
    
    // Setup des tâches
    task[0].addr = &task0;
    task[1].addr = &readSerial;
    task[2].addr = &writeSerial;
    task[3].addr = &WriteMatrice;
    task[4].addr = &Write7Segment;
    
    
    task[0].sp = 0x0300;
    task[1].sp = 0x0400;
    task[2].sp = 0x0500;
    task[3].sp = 0x0600;
    task[4].sp = 0x0700;

    for(uint8_t i = 0; i < NB_TASK; i++){
        initTask(i);
        task[i].state = AWAKE;
    }
 
    // Setup du minuteur 
    initMinuteur(1024,20);
}

int main(){
    setup();
    SP = task[currentTask].sp;
    sei(); 
    task0();
}

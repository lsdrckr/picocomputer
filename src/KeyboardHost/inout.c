#include "inout.h"

volatile uint8_t leds[NB_LED] = {PC7, PC6, PC5, PC4, PC3, PC2, PC1, PC0};
buffer_t buffer;

void initIO(){
    
    // Initialisation des leds
    for(int i=0; i<NB_LED; i++){
        DDRC |= (1<<leds[i]);
    }
    // Initialisation interruption
    DDRB |= (1<<INT);
    setLowOutput(&PORTB, INT);
    
    // Initialisation du buffer 
    buffer.first_index = 0;
    buffer.last_index = -1;
}

void addBuffer(char key){
    buffer.last_index ++;
    if(buffer.last_index >= MAX_DATA) buffer.last_index = 0;
    buffer.data[buffer.last_index] == key;
}

char popBuffer(){
    char key = buffer.data[buffer.first_index];
    buffer.first_index ++;
    if(buffer.first_index >= MAX_DATA) buffer.first_index = 0;
    return key;
}

void setHighOutput(volatile uint8_t *port, volatile uint8_t pin){
    *port |= (1<<pin);
}

void setLowOutput(volatile uint8_t *port, volatile uint8_t pin){
	*port &= ~(1<<pin);
}

void reverseOutput(volatile uint8_t *port, volatile uint8_t pin){
    *port ^= (1<<pin);
}

void printLeds(char c){
    for(int i=0; i<NB_LED; i++){
        if(((c >> i) & 1) == 1){
            setHighOutput(&PORTC, leds[i]);
        }else{
            setLowOutput(&PORTC, leds[i]);
        }
    }
}

void clearLeds(){
    for(int i=0; i<NB_LED; i++){
        setLowOutput(&PORTC, leds[i]);
    }
}

void keyHandler(char key){
    // Sauvegarde de la clef
    addBuffer(key);
    // Envoie de l'interruption 
    setHighOutput(&PORTB, INT);
    _delay_ms(100);
    setLowOutput(&PORTB, INT);
    
}

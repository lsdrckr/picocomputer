#include "inout.h"

volatile uint8_t leds[NB_LED] = {PC7, PC6, PC5, PC4, PC3, PC2, PC1, PC0};

void initLeds(){
    for(int i=0; i<NB_LED; i++){
        DDRC |= (1<<leds[i]);
    }
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


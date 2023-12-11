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
    buffer.head = -1;
    buffer.tail = -1;
}

int isEmpty(){
    return buffer.head == -1;
}

int isFull(FIFO *fifo) {
    return (buffer.tail + 1) % MAX_DATA == fifo.head;
}

int size(){
    if(buffer.tail < buffer.head)
        return MAX_DATA - (buffer.head - buffer.tail);
    return buffer.tail - buffer.head;
}

void enqueue(char key){
    if (isFull(fifo)) {
        return;
    }

    if (isEmpty()) {
        buffer.head = 0;
    }

    buffer.tail = (buffer.tail + 1) % MAX_DATA;
    buffer.data[buffer.tail] = key;
}

char dequeue(){
    if (isEmpty()) {
        return 0x00;
    }

    char key = buffer.data[buffer.head];

    if (buffer.head == buffer.tail) {
        // Le buffer est maintenant vide
        buffer.head = -1;
        buffer.tail = -1;
    } else {
        buffer.head = (buffer.head + 1) % MAX_DATA;
    }

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
    if(size() == 4) 
        setHighOutput(&PORTB, INT);
    else
        setLowOutput(&PORTB, INT);
}

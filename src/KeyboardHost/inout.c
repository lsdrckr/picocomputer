#include "inout.h"

volatile uint8_t leds[NB_LED] = {PC7, PC6, PC5, PC4, PC3, PC2, PC1, PC0};
buffer_t buffer;

void initSPISlave() {
    // Configurer le port MISO comme sortie 
    DDRB |= (1<<MISO);
    // Configurer le port MOSI SCK SS comme entrée
    DDRB &= ~(1 <<MOSI);
    DDRB &= ~(1<<SCK);
    DDRB &= ~(1<<SS);

    // Actvier le mode SPI
    SPCR |= (1<<SPE);

    // Actver mode esclave SPI
    SPCR &= ~(1 << MSTR);

    // Activer l'interruption SPI
    SPCR |= (1 << SPIE);

    // Activer les interruptions globales
    sei();
}

void initIO(){
    // Initialisation des leds
    for(int i=0; i<NB_LED; i++){
        DDRC |= (1<<leds[i]);
    }

    // Initialisation du buffer 
    buffer.head = -1;
    buffer.tail = -1;
    
    // Interruption slave->master
    DDRB |= (1<<INT);
    setLowOutput(&PORTB, INT);

    // Init spi
    initSPISlave();
    sei();
}

uint8_t isEmpty(){
    return buffer.head == -1;
}

uint8_t isFull() {
    return (buffer.tail + 1) % MAX_DATA == buffer.head;
}

uint8_t sizeBuffer(){
    if(isEmpty()) return 0;
    if(buffer.tail < buffer.head)
        return MAX_DATA - (buffer.head - buffer.tail) + 1;
    return buffer.tail - buffer.head + 1;
}

void enqueue(char key){
    if (isFull()) {
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
    enqueue(key);
    // Envoie de l'interruption
    setHighOutput(&PORTB, INT); 
}

ISR(SPI_STC_vect) {
    uint8_t receivedData = SPDR;
    switch (receivedData){
        case 0x00: 
            SPDR = 0x01;
            break;
        case 0x01:
            SPDR = sizeBuffer();
            break;
        default:
            SPDR = 'y';
            break;
    }
    setLowOutput(&PORTB, INT);
}
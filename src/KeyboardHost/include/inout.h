#ifndef __INOUT_H__
#define __INOUT_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define NB_LED 8
#define MISO PB3
#define MOSI PB2
#define SCK PB1
#define SS PB0
#define MAX_DATA 512
#define INT PB4

typedef struct{
    int head;
    int tail;
    char data[MAX_DATA];
}buffer_t;

void initIO(void);
int isEmpty();
int isFull();
int sizeBuffer();
char dequeue();
void setHighOutput(volatile uint8_t *port, volatile uint8_t pin);
void setLowOutput(volatile uint8_t *port, volatile uint8_t pin);
void reverseOutput(volatile uint8_t *port, volatile uint8_t pin);
void printLeds(char c);
void clearLeds(void);
void keyHandler(char key);
#endif

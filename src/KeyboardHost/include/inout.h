#ifndef __INOUT_H__
#define __INOUT_H__

#include <avr/io.h>

#define NB_LED 8
#define MISO PB3
#define MOSI PB2
#define SCK PB1
#define SS PB0


void initLeds(void);
void setHighOutput(volatile uint8_t *port, volatile uint8_t pin);
void setLowOutput(volatile uint8_t *port, volatile uint8_t pin);
void reverseOutput(volatile uint8_t *port, volatile uint8_t pin);
void printLeds(char c);
void clearLeds(void);
#endif

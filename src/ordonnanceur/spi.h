#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define CTC1    WGM12
#define MISO    PB4
#define MOSI    PB3
#define SCK     PB5
#define SS      PB2
#define SS2     PC0
#define SS3     PC3
#define SS4     PD1
#define SS5     PD4
#define SS6     PD7
#define RST1    PC1
#define NB_TASK 5
#define SLEEP   0
#define AWAKE   1

void initSPI();
void selectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss){;
void unselectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss);
uint8_t transferSPI(uint8_t data);
int main();

#endif

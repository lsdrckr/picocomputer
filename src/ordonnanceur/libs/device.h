#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <avr/io.h>
#include "serial.h"
#include <util/delay.h>

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
#define INT2    PC2
#define INT3    PC5
#define INT4    PD3
#define INT5    PD6
#define INT6    PB1
#define MAX_DEVICES 5
#define KEYBOARD 0x01
#define DISPLAY 0x02
#define NETWORK 0x04

typedef struct{
    volatile uint8_t *port;
    volatile uint8_t cs;
    volatile uint8_t *pin;
    volatile uint8_t interrupt;
    uint8_t device;
}he10_t;

// extern he10_t connectorsList[MAX_DEVICES];

void initDevice(void);
void getDeviceList(uint8_t deviceList[MAX_DEVICES]);
uint8_t transferDataTo(uint8_t device, uint8_t data);
int checkInterrupt(uint8_t device);
#endif
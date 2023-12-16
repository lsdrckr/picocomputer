#ifndef __SERIAL_h__
#define __SERIAL_H__

#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate) - 1

#include <avr/io.h>

void initSerial(void);
void serialWrite(uint8_t DataOut);
#endif 
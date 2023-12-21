#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "device.h"

uint8_t data;
uint8_t bufferSize(void);
uint8_t grabKey(void);
void grabKeys(uint8_t keyList[], uint8_t size);

#endif

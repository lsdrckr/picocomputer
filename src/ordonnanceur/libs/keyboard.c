#include "keyboard.h"

uint8_t bufferSize(){
    uint8_t buffer_size;
    transferDataTo(KEYBOARD, 0x01);
    //_delay_ms(100);
    wait(DELAY_SLEEPING, 20);
    buffer_size = transferDataTo(KEYBOARD, 0x01);
    return buffer_size;
}

uint8_t grabKey(){
    uint8_t key;
    transferDataTo(KEYBOARD,0x55);
    //_delay_ms(100);
    wait(DELAY_SLEEPING, 20);
    key = transferDataTo(KEYBOARD,0x55);
    return key;
}

void grabKeys(uint8_t keyList[], uint8_t size){    
    for (int i = 0; i<size; i++){
        //_delay_ms(100);
        wait(DELAY_SLEEPING, 20);
        uint8_t buffer = transferDataTo(KEYBOARD, 0x00);
        keyList[i] = buffer;
    }
}

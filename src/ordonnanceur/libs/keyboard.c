#include "keyboard.h"

uint8_t bufferSize(){
    uint8_t buffer_size;
    transferDataTo(KEYBOARD, 0x01);
    wait(DELAY_SLEEPING, 20);
    buffer_size = transferDataTo(KEYBOARD, 0x01);
    return buffer_size;
}

uint8_t grabKey(){
    uint8_t key;
    transferDataTo(KEYBOARD,0x55);
    wait(DELAY_SLEEPING, 20);
    key = transferDataTo(KEYBOARD,0x55);
    return key;
}

void grabKeys(uint8_t keyList[bufferSize()]){
    uint8_t buffer_size = bufferSize();
    
    for (int i = 0; i<buffer_size; i++){
        wait(DELAY_SLEEPING, 20);
        uint8_t buffer = transferDataTo(KEYBOARD, 0x00);
        keyList[i] = buffer;
    }
}
#include "keyboard.h"

uint8_t bufferSize(){
    uint8_t buffer_size;
    
    transferDataTo(KEYBOARD, 0X01);
    wait(DELAY_SLEEPING,20);
    data = transferDataTo(KEYBOARD, 0xff);
    
    buffer_size = data;
    return buffer_size;
}

uint8_t grabKey(){
    uint8_t key;
    
    transferDataTo(KEYBOARD, 0xff);
    wait(DELAY_SLEEPING,20);
    data = transferDataTo(KEYBOARD, 0xff);

    key = data;
    return key;
}

void grabKeys(uint8_t keyList[], uint8_t size){

    transferDataTo(KEYBOARD, 0x00);
    wait(DELAY_SLEEPING,20);
    data = transferDataTo(KEYBOARD, 0x00);

    for (int i = 0; i<size; i++){
        uint8_t buffer = data;
        data = transferDataTo(KEYBOARD, 0x00);
        keyList[i] = buffer;
    }
}

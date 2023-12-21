#include "keyboard.h"

uint8_t bufferSize(){
    uint8_t buffer_size;
    
    //Attente de la fin de la tâche de la carte
    do{
        data = transferDataTo(KEYBOARD, 0x01);
    }while(data == 0xff){};
    
    buffer_size = data
    return buffer_size;
}

uint8_t grabKey(){
    uint8_t key;
    
    //Attente de la fin de la tâche de la carte
    do{
        data = transferDataTo(KEYBOARD, 0x55);
    }while(data == 0xff){};

    key = data
    return key;
}

void grabKeys(uint8_t keyList[], uint8_t size){
    //Attente de la fin de la tâche de la carte
    do{
        data = transferDataTo(KEYBOARD, 0x00);
    }while(data == 0xff){};
    
    for (int i = 0; i<size; i++){
        uint8_t buffer = data;
        data = transferDataTo(KEYBOARD, 0x00);
        keyList[i] = buffer;
    }
}

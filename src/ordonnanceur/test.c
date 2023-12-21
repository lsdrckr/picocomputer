#include "test.h"

int main(){
    initSerial();
    initSPI();
    // serialWrite('C');
    // serialWrite('O');
    // serialWrite('U');
    // serialWrite('C');
    // serialWrite('O');
    // serialWrite('U');
    // serialWrite(' ');
    // serialWrite('!');
    // serialWrite('\n');
    // serialWrite('\n');
    // serialWrite('\n');
    // serialWrite('\r');
    
    selectSlaveSPI(&PORTC, SS2);
    transferSPI(0x00);
    unselectSlaveSPI(&PORTC, SS2);
    
    // serialPrint("COUCOU !\n\r");
    return 0;
}

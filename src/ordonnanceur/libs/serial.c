#include "serial.h"

void initSerial(void)
{
    // Serial Initialization
    /*Set baud rate 9600 */
    UBRR0H = (unsigned char)((MYUBRR) >> 8);
    UBRR0L = (unsigned char)MYUBRR;
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Frame format: 8data, No parity, 1stop bit */
    UCSR0C = (3 << UCSZ00);
}

unsigned char serialCheckTxReady(void)
{
    return (UCSR0A & _BV(UDRE0)); // nonzero if transmit register is ready to receive new data.
}

void serialWrite(uint8_t DataOut)
{
    while (serialCheckTxReady() == 0) // while NOT ready to transmit
    {
        ;
        ;
    }
    UDR0 = DataOut;
}

void serialPrint(char *s){
    int i = 0;
    while(s[i] != '\0'){
        serialWrite(s[i]);
        i++;
    }
}
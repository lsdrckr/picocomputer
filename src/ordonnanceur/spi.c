#include "spi.h"

void initSPI(){
    // MOSI SCK SS2 SS3 SS4 SS5 SS6 en sortie

    DDRB |= (1<<MOSI);
    DDRB |= (1<<SCK);
    DDRB |= (1<<SS);

    DDRC |= (1<<SS2);
    DDRC |= (1<<SS3);
    DDRD |= (1<<SS4);
    DDRD |= (1<<SS5);
    DDRD |= (1<<SS6);

    // MISO en entrée

    DDRB &= ~(1<<MISO);

    // Sélection d'aucun escalave
    PORTB |= (1<<SS);
    PORTC |= (1<<SS2);
    PORTC |= (1<<SS3);
    PORTD |= (1<<SS4);
    PORTD |= (1<<SS5);
    PORTD |= (1<<SS6);

    // Configurer le registre SPCR (SPI Control Register)
    SPCR |= (1 << SPE) | (1 << MSTR);

    // Configurer la vitesse de transmission Diviseur 64 SPR1 1 SPR0 1 SPI2X 0
    SPCR |= (1 << SPR1); 
    SPCR |= (1 << SPR0);
}

void selectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss){
    // Abaisser la ligne SS pour sélectionner le périphérique
    *ssPort &= ~(1 << ss);
}

void unselectSlaveSPI(volatile uint8_t *ssPort, volatile uint8_t ss){
    // Lever la ligne SS pour désélectionner le périphérique
    *ssPort |= (1 << ss);
}

uint8_t transferSPI(uint8_t data) {
    // Charger les données dans le registre de données
    SPDR = data;

    // Attendre que la transmission soit terminée
    while (!(SPSR & (1 << SPIF)));
    
    // Retourner les données reçues
    return SPDR;
}

int main(){

    initSPI();
    while(1){
        selectSlaveSPI(&PORTC, SS3);
        transferSPI(0x76);
        for(int i=0; i<10; i++){
            transferSPI(i);
            _delay_ms(1000);
        }
        unselectSlaveSPI(&PORTC, SS3);
        _delay_ms(50);
    }
    return 0;
}
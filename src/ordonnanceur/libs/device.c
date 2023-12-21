#include "device.h"

he10_t connectorsList[MAX_DEVICES];

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

    // Configurer la vitesse de transmission Diviseur 128 SPR1 1 SPR0 1 SPI2X 0
    
    SPCR |= (1 << SPR1); 
    SPCR |= (1 << SPR0);
    SPSR |= (1 << SPI2X);
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

void initConnectorsList(){
    uint8_t data;
    connectorsList[0].port = &PORTC;
    connectorsList[0].cs = SS2;
    connectorsList[0].pin = &PINC;
    connectorsList[0].interrupt = INT2;
    
    connectorsList[1].port = &PORTC;
    connectorsList[1].cs = SS3;
    connectorsList[1].pin = &PINC;
    connectorsList[1].interrupt = INT3;
    
    connectorsList[2].port = &PORTD;
    connectorsList[2].cs = SS4;
    connectorsList[2].pin = &PIND;
    connectorsList[2].interrupt = INT4;
    
    connectorsList[3].port = &PORTD;
    connectorsList[3].cs = SS5;
    connectorsList[3].pin = &PIND;
    connectorsList[3].interrupt = INT5;
    
    connectorsList[4].port = &PORTD;
    connectorsList[4].cs = SS6;
    connectorsList[4].pin = &PINB;
    connectorsList[4].interrupt = INT6;
    
    serialPrint("Device : ");

    for(int i = 0; i<MAX_DEVICES; i++){
        selectSlaveSPI(connectorsList[i].port, connectorsList[i].cs);
        transferSPI(0x00);
        data = transferSPI(0x00);
        serialWrite(data+'0');
        connectorsList[i].device = data;
        unselectSlaveSPI(connectorsList[i].port, connectorsList[i].cs);
    }

    serialPrint("\r\n");
}

void initDevice(){
    initSPI();
    initConnectorsList();
}

void getDeviceList(uint8_t deviceList[MAX_DEVICES]){
    for (int i = 0; i < MAX_DEVICES; i++){
        deviceList[i] = connectorsList[i].device;
    }
}

int indexDevice(uint8_t device){
    for(int i = 0; i<MAX_DEVICES; i++){
        if (connectorsList[i].device == device){
            return i;
        }
    }
    return -1;
}

uint8_t transferDataTo(uint8_t device, uint8_t data){
    uint8_t answer;
    int i = indexDevice(device);
    selectSlaveSPI(connectorsList[i].port, connectorsList[i].cs);
    answer = transferSPI(data);
    unselectSlaveSPI(connectorsList[i].port, connectorsList[i].cs);
    return answer;
}

int checkInterrupt(uint8_t device){
    int i = indexDevice(device);
    if (*(connectorsList[i].pin) & (1<<connectorsList[i].interrupt)) return 1;
    return 0;
}

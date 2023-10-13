// D5 PB0 D4 PD4 D3 PD1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void setHighOutput(volatile uint8_t *port, volatile uint8_t pin){
	*port |= (1<<pin);
}

void setLowOutput(volatile uint8_t *port, volatile uint8_t pin){
	*port &= ~(1<<pin);
}

void reverseOutput(volatile uint8_t *port, volatile uint8_t pin){
    *port ^= (1<<pin);
}

void processus1(){
    _delay_ms(100);
    setHighOutput(&PORTD, PD7);
    _delay_ms(100);
    setLowOutput(&PORTD, PD7);
}

void processus2(){
    _delay_ms(100);
    setHighOutput(&PORTD, PD1);
    _delay_ms(100);
    setLowOutput(&PORTD, PD1);
}

void processus3(){
    _delay_ms(100);
    setHighOutput(&PORTD, PD4);
    _delay_ms(100);
    setLowOutput(&PORTD, PD4);
}

ISR(TIMER1_COMPA_vect,ISR_NAKED){
    reverseOutput(&PORTD, PD7);
    asm volatile ( "reti" );
}

void setup(){
    cli();
    
    // Setup des leds
    DDRD |= (1<<PD7);
    DDRD |= (1<<PD1);
    DDRD |= (1<<PD4);
    setLowOutput(&PORTD, PD7);
    setLowOutput(&PORTD, PD1);
    setLowOutput(&PORTD, PD4);
    
    // Setup du vecteur d'interruption TIMER1_COMPA_vect
    TCCR1A = 0x02; // Mode CTC => clear timer on compare match
    TCCR1B = 0x05; // Prédiviseur de 1024
    OCR1A = 15624; // Config valeur de comparaison pour 20 ms
    
    TIMSK1 = (1 << OCIE1A); // Active l'interruption de comparaison A du timer1
    
    sei();
}

int main(){
    setup();
    while(1){
    }
    return 0;
}

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
// Tiny85 Fuses:
// L: 0x91, H: 0xD7, E: 0xFE
// Clock output on PB4
// PLL Clock
// EEPROM is preserved
// Self-Programming is Enabled
// Estimate of OSSCAL, lower bound
#define OSC 166
// Frequency: E5
// Best option, prescaler 64, OCR0A = 236
#define OCR 236

ISR(INT0_vect) {
//  Uncomment to allow manually adjusting OSCCAL
//	OSCCAL++;
}

int main(void) {
	// Disable interrupts
	cli();
	// Detune OSSCAL to 0
	while (OSCCAL > 0) {
		_delay_ms(10);
		OSCCAL--;
	}
	// Stop timer 0
	GTCCR |= (1 << TSM) | (1 << PSR0);
	// Configure CTC mode, output on OC0A
	TCCR0A = (1 << COM0A0) | (1 << WGM01);
	// Prescaler 64
	TCCR0B = (1 << CS01) | (1 << CS00);
	// OCR0A = 236, frequency is E5
	OCR0A = OCR;
	TCNT0 = 0;
	TIMSK = 0;
	// Start timer
	GTCCR &= ~((1 << PSR0) | (1 << TSM));
	// Enable INT0 interrupt on falling edge
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	GIMSK |= (1 << INT0);
	// Enable interrupts
	sei();
	// Everything input, except OC0A
	DDRB = 0x01;
	// Set OSSCAL to a specific start value
	while (OSCCAL <= OSC) {
		_delay_ms(10);
		OSCCAL++;
	}
	for (;;) { // Infinite loop of stalling
		DDRB = 0x01;
	}
	return 0;
}

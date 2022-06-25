#include "SPI.h"

uint8_t __attribute__((noinline)) xmit_spi(uint8_t d) {
	uint8_t tmp = SPDR;
	SPDR = d;
	__builtin_avr_delay_cycles(9);
	return tmp;
}

void __attribute__((noinline)) init_spi(void) {
  // Save a few bytes for 328 CPU - gcc optimizes single bit '|' to sbi.
	PORTB |= 1 << 2;  // SS high
	DDRB  |= 1 << 2;  // SS output mode
	DDRB  |= 1 << 3;  // MOSI output mode
	DDRB  |= 1 << 5;  // SCK output mode
	SD_CS_DDR |= SD_CS_MASK;
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = 1 << SPI2X;
}

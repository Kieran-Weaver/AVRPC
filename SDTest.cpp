// SD test, adapted from https://github.com/greiman/PetitFS/
// For minimum flash use edit pffconfig.h and only enable
// _USE_READ and either _FS_FAT16 or _FS_FAT32

#include "pff/pff.h"
#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

const uint16_t MIN_2X_BAUD = F_CPU/(4*(2*0XFFF + 1)) + 1;
FATFS fs;     /* File system object */
//------------------------------------------------------------------------------

void begin(uint32_t baud) {
	uint16_t baud_setting;
	// don't worry, the compiler will squeeze out F_CPU != 16000000UL
    if ((F_CPU != 16000000UL || baud != 57600) && baud > MIN_2X_BAUD) {
		// Double the USART Transmission Speed
		UCSR0A = 1 << U2X0;
		baud_setting = (F_CPU / 4 / baud - 1) / 2;
    } else {
		// hardcoded exception for compatibility with the bootloader shipped
		// with the Duemilanove and previous boards and the firmware on the 8U2
		// on the Uno and Mega 2560.
		UCSR0A = 0;
		baud_setting = (F_CPU / 8 / baud - 1) / 2;
	}
	// assign the baud_setting
	UBRR0H = baud_setting >> 8;
	UBRR0L = baud_setting;
	// enable transmit and receive
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
}    

int read() {
	if (UCSR0A & (1 << RXC0)) {
		return UDR0;
	}
	return -1;
}

size_t write(uint8_t b) {
	while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}
	UDR0 = b;
	return 1;
}

void write(const uint8_t* data, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		write(data[i]);
	}
}

void print(const char* data) {
	while (*data) {
		write(*(data++));
	}
}

void println(const char* data) {
	print(data);
	write('\n');
}
//------------------------------------------------------------------------------
void errorHalt(const char* msg) {
	print("Error: ");
	println(msg);
	while(1);
}

void test() {
	uint8_t buf[32];
  
	// Initialize SD and file system.
	uint8_t fp = pf_mount(&fs);
	if (fp) errorHalt("pf_mount");
  
	// Open test file.
	if (pf_open("STUFF.TXT")) errorHalt("pf_open");
  
	// Dump test file to Serial.
	while (1) {
		UINT nr;
		if (pf_read(buf, sizeof(buf), &nr)) errorHalt("pf_read");
		if (nr == 0) break;
		write(buf, nr);
	}
}

//------------------------------------------------------------------------------

int main() {
	begin(9600);
	test();
	println("\nDone!");
	return 0;
}

#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <stdint.h>
// SD chip select pin
#define SD_CS_PIN 8

#if SD_CS_PIN < 8
#define SD_CS_PORT PORTD
#define SD_CS_DDR DDRD
#define SD_CS_BIT SD_CS_PIN
#elif SD_CS_PIN < 14
#define SD_CS_PORT PORTB
#define SD_CS_DDR DDRB
#define SD_CS_BIT (SD_CS_PIN - 8)
#elif SD_CS_PIN < 20
#define SD_CS_PORT PORTC
#define SD_CS_DDR DDRC
#define SD_CS_BIT (SD_CS_PIN - 14)
#else  // SD_CS_PIN < 8
#error Bad SD_CS_PIN
#endif  // SD_CS_PIN < 8
#define SD_CS_MASK (1 << SD_CS_BIT)
#define SELECT()  (SD_CS_PORT &= ~SD_CS_MASK)	 /* CS = L */
#define	DESELECT()	(SD_CS_PORT |= SD_CS_MASK)	/* CS = H */
#define	SELECTING	!(SD_CS_PORT & SD_CS_MASK)	  /* CS status (true:CS low) */

//------------------------------------------------------------------------------
/** Send a byte to the card */
uint8_t xmit_spi(uint8_t d);
void init_spi (void);

#ifdef __cplusplus
}
#endif

#endif  // SPI_H

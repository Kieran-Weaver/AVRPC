#ifndef TFT_H
#define TFT_H

#include <stdint.h>

// Low Level TFT Emulator Interface - How the display actually works

// Hidden implementation to obscure ESP32/PC differences
// 0 for column major, 1 for row major
// TFT::begin()
void tft_init( int16_t rows, int16_t cols, bool major );

// TFT::setAddrWindow + setWrite
// Actual display commands use ( left, right ), ( top, bottom )
// Adafruit uses x y w h for no reason
void tft_setRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2 );

// Single pixel
void tft_push1( uint16_t pixel );
// Multiple of the same pixel, len = number of pixels
void tft_pushRLE( uint16_t pixel, uint32_t len );
// 4-bit Palette - 2 pixels per byte, len = number of pixels
void tft_pushPAL4( uint8_t* pixels, uint16_t* pal4, uint32_t len );
// 8-bit Palette - 1 pixel per byte, len = number of pixels
void tft_pushPAL8( uint8_t* pixels, uint16_t* pal8, uint32_t len );
// Direct - 2 pixels per byte, len = number of pixels
void tft_push( uint16_t* pixels, uint32_t len );
// 444 mode, len = number of bytes
void tft_push444( uint8_t* data, uint32_t len );

#endif

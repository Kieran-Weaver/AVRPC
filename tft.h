#ifndef TFT_H
#define TFT_H

#include <stdint.h>

// Low Level TFT Emulator Interface - How the display actually works

// Hidden implementation to obscure ESP32/PC differences
// Portrait / Landscape is in the eye of the beholder,
// "portrait" = "true" means pixels are written in X-major order
// "portrait" = "false" means pixels are written in Y-major order
// it may be useful to set this to the opposite of your actual rendering orientation
//
// The ST7789V supports mirroring and all sorts of crazy bs
// that's not supported by this driver because I don't get how to simulate it
//
// TFT::begin()
void tft_init( bool portrait );

// TFT::setAddrWindow + setWrite
// X is always 0-240, Y is always 0-320
// Does NOT change depending on orientation
void tft_setRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2 );

// Sets the fixed areas and scroll area
//
// |-------------------------|
// |                         |} <- top fixed area (TFA)
// |-------------------------|
// |                         |}
// |     scroll direction    |}
// |           |             |} <- vertical scroll area (VSA)
// |           v             |}
// |                         |}
// |-------------------------|
// |                         |} <- bottom fixed area (BFA)
// |-------------------------|
//
// only the scroll area scrolls.
// TFA + VSA + BFA must be 320, so this only happens in the "Y" direction
// if you want landscape, rotate your screen 90 degrees and look at the diagram again
void tft_setScrollWindow( int16_t tfa, int16_t vsa, int16_t bfa );

// Sets the line to display at the top of the VSA
// absolute coordinates, not relative
// must be within the VSA
void tft_scroll( uint16_t scroll );

// Invert colours
void tft_invert( bool invert );

// Single pixel
void tft_push1( uint16_t pixel );
// Multiple of the same pixel, len = number of pixels
void tft_pushRLE( uint16_t pixel, uint32_t len );
// Direct - 2 pixels per byte, len = number of pixels
void tft_push( uint16_t* pixels, uint32_t len );
// 4-bit Palette - 2 pixels per byte, len = number of pixels
// the first pixel is the HIGH nibble
void tft_pushPAL4( uint8_t* pixels, uint16_t* pal4, uint32_t len );
// 8-bit Palette - 1 pixel per byte, len = number of pixels
void tft_pushPAL8( uint8_t* pixels, uint16_t* pal8, uint32_t len );
// 444 mode, len = number of pixels, must be even
// In the actual display, you can use this as a 64-color palette
// ( that's not supported here )
void tft_push444( uint8_t* data, uint32_t len );

// start of frame, end of previous frame
// this "portrait" is completely independent of the other one
// and corresponds to how the user rotates the display
// returns number of SPI bytes written
uint32_t tft_draw( bool portrait );

void tft_shut();
#endif

#include "gfx.h"
#include "tft.h"
#include "draw.h"
#include "string.h"

fb::~fb() {
	if ( pixels ) {
		for ( int i = 0; i < h; i++ ) {
			delete[] pixels[ i ];
		}
	}

	delete[] pixels;
}

bool fb::init( int16_t w, int16_t h ) {
	this->w = w;
	this->h = h;
	pixels = new uint16_t*[ h ]; 
	if ( !pixels ) return false;

	for ( int y = 0; y < h; y++ ) {
		pixels[ y ] = new uint16_t[ w ];
		if ( !pixels[ y ] ) return false;
		memset( pixels[ y ], 0, w * 2 );
	}

	return true;
}

void fb::writePixel( int16_t x, int16_t y, uint16_t color ) {
	pixels[ y ][ x ] = color;
}

void fb::writeFastHLine( int16_t x1, int16_t y, int16_t x2, uint16_t color ) {
	for ( int x = x1; x < x2; x++ ) {
		writePixel( x, y, color );
	}
}

void fb::writeFastVLine( int16_t x, int16_t y1, int16_t y2, uint16_t color ) {
	for ( int y = y1; y < y2; y++ ) {
		writePixel( x, y, color );
	}
}

void fb::drawRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ) {
	writeFastHLine( x1, y1, x2, color );
	writeFastHLine( x1, y2, x2, color );
	writeFastVLine( x1, y1, y2, color );
	writeFastVLine( x2, y1, y2, color );
}

void fb::fillRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ) {
	for ( int y = y1; y < y2; y++ ) {
		for ( int x = x1; x < x2; x++ ) {
			writePixel( x, y, color );
		}
	}
}

void fb::fillScreen( uint16_t color ) {
	fillRect( 0, 0, w, h, color );
}

void fb::push( void ) {
	tft_setRect( 0, 0, 239, 319 );
	for ( int y = 0; y < h; y++ ) {
		tft_push( pixels[ y ], w );
	}
}

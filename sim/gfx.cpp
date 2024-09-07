#include "gfx.h"
#include "tft.h"
#include "draw.h"
#include "string.h"
#include <math.h>

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
	if ( ( x >= 0 ) && ( x < w ) && ( y >= 0 ) && ( y < h ) ) {
		pixels[ y ][ x ] = color;
	}
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

void fb::drawLine( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ) {
	int dx = abs( x2 - x1 );
	int sx = x1 < x2 ? 1 : -1;
	int dy = -abs( y2 - y1 );
	int sy = y1 < y2 ? 1 : -1;
	int error = dx + dy;
	while ( 1 ) {
		writePixel( x1, y1, color );
		if ( ( x1 == x2 ) && ( y1 == y2 ) ) break;
		int e2 = 2 * error;
		if ( e2 >= dy ) {
			error += dy;
			x1 += sx;
		}
		if ( e2 <= dx ) {
			error += dx;
			y1 += sy;
		}
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

#include "tft.h"
#include "draw.h"
#include <assert.h>
#include <string.h>

struct TFT {
// TFT DRAM
	uint16_t pixels[ 320 * 240 ];
// TFT attributes
	bool portrait;
	bool invert;
// Drawing position
	int x;
	int y;
// Current rect
	int x1;
	int y1;
	int x2;
	int y2;
// Scroll settings
	int tfa;
	int vsa;
	int bfa;
	int scroll;
// Debug info
	uint32_t spiBytes;
};

static TFT tft;

void tft_init( bool portrait ) {
	tft = {};
	tft.vsa = 320;
	tft.portrait = portrait;
	memset( tft.pixels, 0, sizeof( tft.pixels ) );
	draw_init();
}

void tft_setRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2 ) {
	// If you break these conditions in the actual display
	// it's Undefined Behaviour
	assert( ( x1 >= 0 ) && ( x1 < 240 ) );
	assert( ( x2 >= 0 ) && ( x2 < 240 ) );
	assert( ( y1 >= 0 ) && ( y1 < 320 ) );
	assert( ( y2 >= 0 ) && ( y2 < 320 ) );
	assert( x2 > x1 );
	assert( y2 > y1 );

	tft.x1 = x1;
	tft.x2 = x2;
	tft.y1 = y1;
	tft.y2 = y2;

	tft.x = x1;
	tft.y = y1;

	tft.spiBytes += 2 * sizeof( tft.x ) + 2 * sizeof( tft.y );
}

void tft_setScrollWindow( int16_t tfa, int16_t vsa, int16_t bfa ) {
	tft.tfa = tfa;
	tft.vsa = vsa;
	tft.bfa = bfa;

	assert( ( tfa + vsa + bfa ) == 320 );

	tft.spiBytes += 3 * sizeof( tfa );
}

void tft_scroll( uint16_t scroll ) {
	tft.scroll = scroll;

	assert( ( scroll >= tft.tfa ) && ( scroll <= ( tft.tfa + tft.vsa ) ) );

	tft.spiBytes += sizeof( scroll );
}

void tft_invert( bool invert ) {
	tft.invert = invert;

	tft.spiBytes += 1;
}

void tft__push1( uint16_t pixel ) {
	// Nothing has broken internally
	assert( ( tft.x >= tft.x1 ) && ( tft.x <= tft.x2 ) );
	assert( ( tft.y >= tft.y1 ) && ( tft.y <= tft.y2 ) );

	tft.pixels[ tft.y * 240 + tft.x ] = pixel;

	if ( tft.portrait ) {
		tft.x++;
		if ( tft.x > tft.x2 ) {
			tft.y++;
			tft.x = tft.x1;
		}
	} else {
		tft.y++;
		if ( tft.y > tft.y2 ) {
			tft.x++;
			tft.y = tft.y1;
		}
	}
}

void tft_push1( uint16_t pixel ) {
	tft__push1( pixel );
	tft.spiBytes += 2;
}

void tft_pushRLE( uint16_t pixel, uint32_t len ) {
	// A real driver would optimize this to be memcpy or 1 SPI push
	for ( int i = 0; i < len; i++ ) {
		tft_push1( pixel );
	}
}

void tft_push( uint16_t* pixels, uint32_t len ) {
	for ( int i = 0; i < len; i++ ) {
		tft_push1( pixels[ i ] );
	}
}

void tft_pushPAL4( uint8_t* pixels, uint16_t* pal4, uint32_t len ) {
	for ( int i = 0; i < ( len / 2 ); i++ ) {
		uint8_t tmp = *(pixels++);
		uint8_t idx1 = ( tmp & 0xf0 ) >> 4;
		uint8_t idx2 = tmp & 0x0f;
		tft_push1( pal4[ idx1 ] );
		tft_push1( pal4[ idx2 ] );
	}
	
	if ( len & 1 ) {
		uint8_t idx = ( (*pixels) & 0xf0 ) >> 4;
		tft_push1( pal4[ idx ] );
	}
}

void tft_pushPAL8( uint8_t* pixels, uint16_t* pal8, uint32_t len ) {
	for ( int i = 0; i < len; i++ ) {
		tft_push1( pal8[ pixels[ i ] ] );
	}
}

void tft_push444( uint8_t* data, uint32_t len ) {
	assert( ( len & 1 ) == 0 );

	for ( int i = 0; i < ( len / 2 ); i++ ) {
		uint8_t bytes[ 3 ];
		bytes[ 0 ] = *(data++);
		bytes[ 1 ] = *(data++);
		bytes[ 2 ] = *(data++);

		uint16_t pixel;
		pixel += ( bytes[ 0 ] & 0xf0 ) >> 4;
		pixel = pixel << 5;
		pixel += ( bytes[ 0 ] & 0x0f );
		pixel = pixel << 6;
		pixel += ( bytes[ 1 ] & 0xf0 ) >> 4;
		pixel = pixel << 1;
		tft__push1( pixel );

		pixel += ( bytes[ 1 ] & 0x0f );
		pixel = pixel << 5;
		pixel += ( bytes[ 2 ] & 0x0f ) >> 4;
		pixel = pixel << 6;
		pixel += ( bytes[ 2 ] & 0x0f );
		pixel = pixel << 1;
		tft__push1( pixel );

		tft.spiBytes += 3;
	}
}

uint32_t tft_draw( bool portrait ) {
	uint32_t tmp = tft.spiBytes;

	uint16_t * newpixels = new uint16_t[ 240 * 320 ];

	memcpy( newpixels, tft.pixels, 480 * tft.tfa );

	int iny = tft.scroll;
	for ( int y = tft.tfa; y < ( tft.tfa + tft.vsa ); y++ ) {
		memcpy( newpixels + ( y * 240 ), tft.pixels + ( iny * 240 ), 480 );
		iny++;
		if ( iny >= ( tft.tfa + tft.vsa ) ) iny = tft.tfa;
	}

	memcpy( newpixels + ( ( tft.tfa + tft.vsa ) * 240 ), tft.pixels + ( ( tft.tfa + tft.vsa ) * 240 ), tft.bfa * 480 );

	if ( tft.invert ) {
		for ( int i = 0; i < ( 320 * 240 ); i++ ) {
			newpixels[ i ] = ~ newpixels[ i ];
		}
	}

	draw_draw( portrait, newpixels );
	tft.spiBytes = 0;

	delete[] newpixels;

	return tmp;
}

void tft_shut() {
	draw_shut();
}

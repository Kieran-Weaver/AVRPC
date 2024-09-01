#ifndef GFX_H
#define GFX_H

#include <stdint.h>

// Framebuffer interface

struct fb {
	fb() = default;
	// No copying
	fb( const struct fb& other ) = delete;
	~fb();
	bool init( int16_t w, int16_t h );
	void writePixel( int16_t x, int16_t y, uint16_t color );
	void writeFastVLine( int16_t x, int16_t y1, int16_t y2, uint16_t color );
	void writeFastHLine( int16_t x1, int16_t y, int16_t x2, uint16_t color );
	void drawRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
	void fillRect( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
	void fillScreen( uint16_t color );

	// Pushes to TFT
	void push( void );
private:
	int16_t w;
	int16_t h;
	uint16_t **pixels;
};

#endif

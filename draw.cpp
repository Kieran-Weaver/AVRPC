#include "draw.h"
#include <stdlib.h>
#include <assert.h>

#ifndef _WIN32
#include <SDL.h>

/* SDL Boilerplate stuff */

#define SCALE 3

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static int lastw;
static int lasth;

void draw_init( void ) {
	assert( SDL_Init( SDL_INIT_VIDEO ) >= 0 && "SDL_Init failed" );
	window = SDL_CreateWindow( "ESP32 / AVRPC5", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 320 * SCALE, 240 * SCALE, SDL_WINDOW_SHOWN );
	assert( !!window && "Failed to create window" );
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	assert( !!renderer && "Failed to create renderer" );
	lastw = -1;
	lasth = -1;
	texture = nullptr;	
}

void draw_draw( bool portrait, const uint16_t* pixels ) {
	int w;
	int h;

	if ( portrait ) {
		w = 240;
		h = 320;
	} else {
		w = 320;
		h = 240;
	}

	if ( ( w != lastw ) || ( h != lasth ) ) {
		SDL_SetWindowSize( window, w * SCALE, h * SCALE );
		SDL_RenderSetLogicalSize( renderer, w, h );
		if ( texture != nullptr ) {
			SDL_DestroyTexture( texture );
		}
		texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, w, h );
		assert( !!texture && "Failed to create texture" );
	}

	SDL_Rect screen = { 0, 0, w, h };
	uint16_t* dst = nullptr;
	int pitch = 0;
	SDL_LockTexture( texture, &screen, (void**)&dst, &pitch );

	for ( int i = 0; i < h; i++ ) {
		memcpy( dst, pixels, w * 2 );
		pixels += w;
		dst += ( pitch / 2 );
	}

	SDL_UnlockTexture( texture );

	SDL_Vertex tl = { { 0, 0 }, { 255, 255, 255, 255 }, { 0, 0 } };
	SDL_Vertex tr = { { w, 0 }, { 255, 255, 255, 255 }, { 1, 0 } };
	SDL_Vertex bl = { { 0, h }, { 255, 255, 255, 255 }, { 0, 1 } };
	SDL_Vertex br = { { w, h }, { 255, 255, 255, 255 }, { 1, 1 } };

	SDL_Vertex tris[] = { tl, tr, bl, bl, tr, br };

	SDL_RenderClear( renderer );
	SDL_RenderGeometry( renderer, texture, tris, 6, NULL, 0 );
	SDL_RenderPresent( renderer );
}

void draw_shut() {
	SDL_DestroyTexture( texture );
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	SDL_Quit();
}

bool draw_done(void) {
	SDL_Event e;
	SDL_PollEvent( &e );
	if ( e.type == SDL_QUIT )
		return true;
	return false;
}

#else
/* Win32 API code */

#include <windows.h>
#include <GL/gl.h>

#endif

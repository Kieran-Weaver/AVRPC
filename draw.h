#ifndef TFT_SDL2_H
#define TFT_SDL2_H

#include <stdint.h>

void draw_init( void );
bool draw_done( void );
void draw_draw( bool portrait, const uint16_t* pixels );
void draw_shut( void );

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MAIN WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
#else
#define MAIN main( void )
#endif

#endif

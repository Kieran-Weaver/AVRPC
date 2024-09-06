#ifndef TFT_SDL2_H
#define TFT_SDL2_H

#include <stdint.h>

void draw_init( void );
bool draw_done( void );
void draw_draw( bool portrait, const uint16_t* pixels );
void draw_shut( void );

#endif

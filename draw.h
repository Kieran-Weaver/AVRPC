#ifndef TFT_SDL2_H
#define TFT_SDL2_H

#include <stdint.h>

struct touch {
	bool touched;
	int x;
	int y;
};

void draw_init( void );
bool draw_done( void );
void draw_draw( bool portrait, const uint16_t* pixels );
touch draw_touch( void );
void draw_shut( void );

#endif

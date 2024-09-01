#include "tft.h"
#include <SDL.h>

#define BLACK 0
#define RED 0xF800

int main( int argc, char **argv ) {
	tft_init( false );
	bool quit = false;
	
	while ( !quit ) {
		SDL_Event e;
		SDL_PollEvent( &e );
		if ( e.type == SDL_QUIT )
			quit = true;

		// Drawing code goes here
		tft_setRect( 0, 0, 239, 319 );
		tft_pushRLE( RED, 240 * 320 );
		// Drawing code end

		tft_draw( false );

	}

	tft_shut();
	
	return 0;
}

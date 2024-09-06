#include "tft.h"
#include "gfx.h"
#include "draw.h"

#define WHITE 0xFFFF
#define BLACK 0
#define RED 0xF800

int main( void ) {
	tft_init( true );
	bool quit = false;
	fb FB;

	FB.init( 320, 240 );
	int i = 0;

	while ( !quit ) {
		// Drawing code goes here
		FB.fillScreen( WHITE );
		FB.drawRect( 20, 20, 300, 220, BLACK );
		FB.fillRect( 40, 40, 280, 200, i );
		i++;

		FB.push();
		// Drawing code end

		tft_draw( false );

		if ( draw_done() )
			quit = true;

	}

	tft_shut();
	
	return 0;
}

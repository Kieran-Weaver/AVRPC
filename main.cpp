#include "tft.h"
#include "gfx.h"
#include "draw.h"

#define WHITE 0xFFFF
#define BLACK 0
#define RED 0xF800

int main( int argc, char **argv ) {
	tft_init( true );
	bool quit = false;
	fb FB;

	FB.init( 320, 240 );
	
	while ( !quit ) {
		if ( draw_done() )
			quit = true;

		// Drawing code goes here
		FB.fillScreen( WHITE );
		FB.drawRect( 20, 20, 300, 220, BLACK );
		FB.fillRect( 40, 40, 280, 200, RED );

		FB.push();
		// Drawing code end

		tft_draw( false );

	}

	tft_shut();
	
	return 0;
}

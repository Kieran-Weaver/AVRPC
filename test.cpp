#include "sim/tft.h"
#include "sim/gfx.h"
#include "sim/draw.h"

#define WHITE 0xFFFF
#define BLACK 0
#define RED 0xF800

int main( void ) {
	tft_init( false );
	fb FB;

	FB.init( 320, 240 );
	int i = 0;
	int s = 30;

	tft_setScrollWindow( 30, 260, 30 );
	tft_scroll( s );

	while ( !draw_done() ) {

		// Drawing code goes here
		FB.fillScreen( WHITE );
		FB.drawRect( 20, 20, 300, 220, BLACK );
		FB.fillRect( 40, 40, 280, 200, i );
		i++;

		FB.push();

		if ( ( i % 16 ) == 0 ) {
			s++;
			tft_scroll( s );
		}
		// Drawing code end

		tft_draw( false );

	}

	tft_shut();
	
	return 0;
}


#include "tft.h"
#include "gfx.h"
#include "draw.h"

#define BLUE 0x001F

struct vec2 {
	int x;
	int y;
};

struct vec3 {
	int x;
	int y;
	int z;
};

struct vec3 vertices[8] = {
 	{ -75, -75, -75 },
 	{ 75, -75, -75 },
 	{ 75, -75, 75 },
 	{ -75, -75, 75 },
 	{ -75, 75, -75 },
 	{ 75, 75, -75 },
 	{ 75, 75, 75 },
 	{ -75, 75, 75 },
 };

vec2 project( vec3& camera, vec3& point, int window_dist ) {
	int tmp = ( 100 * window_dist ) / ( point.z - camera.z );
	return { ((point.x - camera.x) * tmp) / 100 + 160, ((point.y - camera.y) * tmp) / 100 + 120 };
}

void render_cube( fb& FB, int j, int k ) {
	FB.fillScreen( 0xFFFF );
	vec2 projected_vertices[8];
	int window_dist = 400;
	vec3 camera = { j, k, -500 };
	
	for ( int i = 0; i < 8; i++ ) {
		projected_vertices[i] = project( camera, vertices[ i ], window_dist );
	}

	for ( int i = 0; i < 4; i++ ) {
		FB.drawLine( projected_vertices[i].x, projected_vertices[i].y, projected_vertices[(i + 1) % 4].x, projected_vertices[(i + 1) % 4].y, BLUE);
		FB.drawLine( projected_vertices[i + 4].x, projected_vertices[i + 4].y, projected_vertices[((i + 1) % 4) + 4].x, projected_vertices[((i + 1) % 4) + 4].y, BLUE);
		FB.drawLine( projected_vertices[i].x, projected_vertices[i].y, projected_vertices[i + 4].x, projected_vertices[i + 4].y, BLUE);
	}
}

int main( void ) {
	tft_init( true );
	fb FB;

	FB.init( 240, 320 );

	int phase = 0;
	int speed = 5;
	int j = 0;
	int k = 0;

	while ( !draw_done() ) {
		
		// Drawing code goes here
		switch ( phase ) {
			case 0:
				j += speed;
				k = 0;
				if ( j >= 200 ) phase = 1;
				break;
			case 1:
				j = 200;
				k -= speed;
				if ( k <= -200 ) phase = 2;
				break;
			case 2:
				k = -200;
				j -= speed;
				if ( j <= -200 ) phase = 3;
				break;
			case 3:
				j = -200;
				k += speed;
				if ( k >= 0 ) phase = 4;
				break;
			case 4:
				k = 0;
				j += speed;
				if ( j >= 0 ) {
					phase = 0;
					j = 0;
				}
				break;
		}

		render_cube( FB, j, k );

		FB.push();

		// Drawing code end

		tft_draw( false );

	}

	tft_shut();
	
	return 0;
}

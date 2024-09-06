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
		lastw = w;
		lasth = h;
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static volatile bool initialized;
static HWND window;
static volatile bool done = false;
static uint32_t* px32 = nullptr;
static volatile bool _port = false;
static int lastw, lasth;

static void win32_init( HDC hdc ) {
	PIXELFORMATDESCRIPTOR pdf = {
		.nSize = sizeof(pdf),
		.nVersion = 1,
		.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_GDI,
		.iPixelType = PFD_TYPE_RGBA,
		.cColorBits = 32,
		.cDepthBits = 24,
		.cStencilBits = 8,
		.iLayerType = PFD_MAIN_PLANE,
	};
	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pdf), &pdf);
	px32 = new uint32_t[ 320 * 240 ];

	initialized = true;
}

static void doPaint( HDC hdc ) {
	BITMAPINFOHEADER bmih = { 0 };
	bmih.biSize = sizeof( BITMAPINFOHEADER );
	bmih.biWidth = ( _port ? 240 : 320 );
	bmih.biHeight = ( _port ? -320 : -240 );
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = 0;
	bmih.biXPelsPerMeter = 10;
	bmih.biYPelsPerMeter = 10;

	BITMAPINFO dbmi = { 0 };
	dbmi.bmiHeader = bmih;

	StretchDIBits( hdc, 0, 0, bmih.biWidth * 3, -bmih.biHeight * 3, 0, 0, bmih.biWidth, -bmih.biHeight, px32, &dbmi, 0, SRCCOPY );
}

static LRESULT CALLBACK
win32_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg) {
		case WM_CREATE:
			win32_init(GetDC(hwnd));
			break;
		case WM_PAINT:
			hdc = BeginPaint( hwnd, &ps );
			doPaint( hdc );
			EndPaint( window, &ps );
			break;
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage( 0 );
			done = true;
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

void draw_init( void ) {
	const char* title = "ESP32 / AVRPC5";
	WNDCLASS wndclass = {
		.lpfnWndProc = win32_wndproc,
		.lpszClassName = "gl",
	};
	RegisterClass( &wndclass );
	DWORD style = WS_OVERLAPPED | WS_VISIBLE;
	window = CreateWindow( "gl", title, style, 0, 0, 720, 960, 0, 0, 0, 0 );
}

uint32_t rgb565to888( uint16_t pixel ) {
	uint32_t c;
	c = ( ( pixel >> 11 ) * 527 + 23 ) >> 6;
	c = (c << 8) | ( ( ( pixel >> 5 ) & 0x3f ) * 259 + 33 ) >> 6;
	c = (c << 8) |( ( pixel & 0x1f ) * 527 + 23 ) >> 6;
	return c;
}

void draw_draw( bool portrait, const uint16_t* pixels ) {
	if (!initialized) assert( false );

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
		lastw = w;
		lasth = h;
		SetWindowPos( window, NULL, 0, 0, w * 3, h * 3, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
	}

	for (int i = 0; i < (320*240); i++ ) {
		px32[ i ] = rgb565to888( pixels[ i ] );
	}

	_port = portrait;

	RedrawWindow( window, NULL, NULL, RDW_INTERNALPAINT );
}

void draw_shut( void ) {
}

bool draw_done( void ) {
	MSG msg;
	while ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) == TRUE ) {
		if ( msg.message == WM_QUIT )
			break;
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return done;
}

#endif

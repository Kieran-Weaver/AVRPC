#ifndef TFT_ILI9163_HPP
#define TFT_ILI9163_HPP
#include <vector>
#include <array>
#include <cstdint>
enum ILI9163_COMMANDS {
	TFT_NOP     = 0x00, // No-op
	TFT_RESET   = 0x01, // Software Reset
	TFT_SLPIN   = 0x10, // Sleep in
	TFT_SLPOUT  = 0x11, // Sleep out
	TFT_PTLON   = 0x12, // Partial mode on
	TFT_NORON   = 0x13, // Normal display mode on
	TFT_INVOFF  = 0x20, // Invert off
	TFT_INVON   = 0x21, // Invert on
	TFT_GAMMSET = 0x26, // Gamma set
	TFT_DISPOFF = 0x28, // Display off
	TFT_DISPON  = 0x29, // Display on
	TFT_CASET   = 0x2A, // Column address set
	TFT_PASET   = 0x2B, // Page address set
	TFT_RAMWR   = 0x2C, // Memory Write
	TFT_COLSET  = 0x2D, // Color mode set
	TFT_PTLAR   = 0x30, // Partial area set
	TFT_VSCRL   = 0x33, // Vertical scroll
	TFT_MACTL   = 0x36, // Memory Access Control
	TFT_VSSA    = 0x37, // Vertical scroll start address
	TFT_IDLEON  = 0x38, // Idle on
	TFT_IDLEOFF = 0x39, // Idle off
	TFT_IPA     = 0x3A, // Interface pixel format
	TFT_FRCFULL = 0xB1, // Framerate control normal mode
	TFT_FRCIDLE = 0xB2, // Framerate control idle mode
	TFT_FRCPTL  = 0xB3, // Framerate control partial mode
	TFT_INVCTL  = 0xB4, // Display inversion control
	TFT_SDDC    = 0xB7, // Source driver direction control
	TFT_GDDC    = 0xB8, // Gate driver direction control
	TFT_PWRCTL1 = 0xC0, // Power control 1
	TFT_PWRCTL2 = 0xC1, // Power control 2
	TFT_PWRCTL3 = 0xC2, // Power control 3
	TFT_PWRCTL4 = 0xC3, // Power control 4
	TFT_PWRCTL5 = 0xC4, // Power control 5
	TFT_VCMCTL1 = 0xC5, // VCOM control 1
	TFT_VCMCTL2 = 0xC6, // VCOM control 2
	TFT_VCOMOFF = 0xC7, // VCOM offset control
	TFT_ID4WR   = 0xD3, // Write ID4 control
	TFT_NVMFC   = 0xD7, // NV memory function controller
	TFT_NVMFC2  = 0xDE, // NV memory function controller 2
	TFT_PGAMCOR = 0xE0, // Positive gamma correction setting
	TFT_NGAMCOR = 0xE1, // Negative gamma correction setting
	TFT_GAMRSEL = 0xF2  // GAM_R_SEL
};

enum ILI9163_BPP {
	TFT_RGB444 = 3,
	TFT_RGB565 = 5,
	TFT_RGB666 = 6
};

struct TFT_State {
	uint16_t w;
	uint16_t h;
	bool inverted;
	bool sleep;
	bool idle;
	bool xflip;
	bool yflip;
	
	bool scrolling;
	// TFA + VSA + BFA must equal 1
	float TFA; // Top fixed area
	float VSA; // Vertical scroll area
	float BFA; // Bottom fixed area
	// SSA describes first line in VSA
	float SSA; // Scroll start address
};

class TFT_ILI9163{
	public:
// Universal high-level API
		TFT_ILI9163();
		void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void pushColor(uint16_t color);
// Universal low-level API
		void writeCommand(ILI9163_COMMANDS command);
		void writeData(uint8_t data);
// Helper functions
		void pushColor(uint16_t color, uint32_t count);
		void drawPixel(uint16_t x, uint16_t y, uint16_t color);
		void drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
		void drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
		void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
		void fillScreen(uint16_t color);
// Simulator functions
		void setSize(uint16_t w, uint16_t h);			// Set the size of the display
		const std::vector<uint32_t>& getPixels() const;	// Get current framebuffer
		const TFT_State& getState() const;              // Get rendering state
	private:
		int writeRGB(uint32_t* pixels, const uint8_t* buf);
// Data for current frame
		std::array<int16_t, 4> addrWindow = {}; // Current rectangle where pixels are written to
		std::array<uint8_t, 3> pixelBuf = {};    // SPI Buffer for pixels
		int8_t pixelIdx = 0;
		std::vector<uint32_t> pixelData; 	     // Color(X, Y) = pixelData[Y*W + X]
		int16_t x = 0;
		int16_t y = 0;
		ILI9163_COMMANDS command; // Current command
		ILI9163_BPP bpp = TFT_RGB565; // Current bpp
		uint16_t cmdnum;          // Current data byte of command
// Rendering state
		TFT_State state = {
			160, 128,                 // Dimensions
			false, false, false,      // Inverted, sleep, idle
			false, false,             // X-flip, Y-flip
			false, 1.f, 0.f, 0.f, 0.f // Scroll settings
		};
// Internal registers
		uint16_t TFA; // Top fixed area
		uint16_t VSA; // Vertical scroll area
		uint16_t BFA; // Bottom fixed area
		uint16_t SSA; // Scroll start address
		bool MY = false; // Row Address Order
		bool MX = false; // Column Address Order
		bool MV = false; // Page / Column Selection
		bool ML = false; // Vertical Order
		bool RGB = false; // 0 = RGB, 1 = BGR
		bool MH = false; // Display data latch order
};
#endif

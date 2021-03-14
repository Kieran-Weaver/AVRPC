#ifndef TFT_ILI9163_HPP
#define TFT_ILI9163_HPP
#include <vector>
#include <array>
#include <cstdint>
class TFT_ILI9163{
	public:
// Universal high-level API
		TFT_ILI9163();
		void begin();
		void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void pushColor(uint16_t color);
// Universal low-level API
		void writeCommand(uint8_t command);
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
		void refresh();									// End the last frame and start a new one
		uint64_t getCycles() const;						// Get cycles spent drawing during last frame
		const std::vector<uint16_t>& getPixels() const;	// Get pixels written to display during last frame
		void getDims(uint16_t& w, uint16_t& h) const;	// Get dimensions of screen
	private:
// Simulator parameters
		uint8_t data_cycles = 18;			// Cycles required to write 1 byte of data
// Data for current frame
		uint64_t cycle_count = 0;			// Cycles spent drawing: Only accurate for AVR8
		std::array<uint16_t, 4> addrWindow;	// Current rectangle where pixels are written to
		std::vector<uint16_t> pixelData; 	// Color(X, Y) = pixelData[Y*W + X]
		uint16_t x = 0;
		uint16_t y = 0;
		uint16_t w = 0;
		uint16_t h = 0;
// Data for old frame
		uint64_t last_count = 0;
		std::vector<uint16_t> oldPixels;
		uint16_t oldw = 0;
		uint16_t oldh = 0;
};
#endif

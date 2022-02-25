#include "TFT_ILI9163.h"
#include <cstddef>

TFT_ILI9163::TFT_ILI9163(){
	this->setSize(128, 160);
}

void TFT_ILI9163::setSize(uint16_t w, uint16_t h){
	this->w = w;
	this->h = h;
	this->pixelData.resize(w*h);
}

void TFT_ILI9163::writeCommand(ILI9163_COMMANDS command){
	this->command = command;
	this->cmdnum = 0;

	switch (command) {
	case TFT_SLPIN:
	case TFT_DISPOFF:
		sleep = true;
		break;
	case TFT_SLPOUT:
	case TFT_DISPON:
		sleep = false;
		break;
	case TFT_IDLEON:
		idle = true;
		break;
	case TFT_IDLEOFF:
		idle = false;
		break;
	case TFT_RESET:
		this->pixelData.clear();
		this->pixelData.resize(w*h);
		break;
	case TFT_INVOFF:
		this->inverted = false;
		break;
	case TFT_INVON:
		this->inverted = true;
		break;
	case TFT_RAMWR:
		this->x = addrWindow[0];
		this->y = addrWindow[2];
		this->pixelIdx = 0;
		break;
	default:
		break;
	}
}

void TFT_ILI9163::writeData(uint8_t data){
	const std::array<uint16_t, 2> masks = { 0x00FF, 0xFF00 };
	const std::array<uint16_t, 2> shifts = { 8, 0 };

	switch (this->command) {
	// case TFT_COLSET unsupported
	case TFT_VSCRL: // Vertical Scroll
		break;
	case TFT_MACTL: // Memory Access Control
		break;
	case TFT_VSSA: // Vertical Scroll Start Address
		break;
	case TFT_IPA: // Interface Pixel Format
		break;
	case TFT_INVCTL: // Display Inversion Control
		break;
	// Invert X and Y direction
	case TFT_SDDC: // Source driver direction control
		break;
	case TFT_GDDC: // Gate driver direction control
		break;
	case TFT_CASET:
		addrWindow[cmdnum / 2] &= masks[cmdnum % 2];
		addrWindow[cmdnum / 2] |= data << shifts[cmdnum % 2];
		this->x = addrWindow[0];
		break;
	case TFT_PASET:
		addrWindow[cmdnum / 2 + 2] &= masks[cmdnum % 2];
		addrWindow[cmdnum / 2 + 2] |= data << shifts[cmdnum % 2];
		this->y = addrWindow[2];
		break;
	case TFT_RAMWR:
		this->pixelBuf[this->pixelIdx++] = data;
		if (((bpp == TFT_RGB565) && (pixelIdx == 2)) || (pixelIdx == 3)) {
			this->x += this->writeRGB(pixelData.data() + (y * w + x), pixelBuf.data());
			this->pixelIdx = 0;
		}
		
		if (this->x > addrWindow[1]) {
			this->x = addrWindow[0];
			this->y++;
			if (this->y > addrWindow[3]) this->y = addrWindow[2];
		}
		
		break;
	}
	
	this->cmdnum++;
}

const std::vector<uint32_t>& TFT_ILI9163::getPixels() const{
	return this->pixelData;
}

void TFT_ILI9163::pushColor(uint16_t color){
	this->writeData(color >> 8);
	this->writeData(color & 0xFF);
}

void TFT_ILI9163::pushColor(uint16_t color, uint32_t count){
	for (int i = 0; i < count; i++){
		this->pushColor(color);
	}
}

void TFT_ILI9163::fillScreen(uint16_t color){
	this->fillRect(0, 0, w, h, color);
}

void TFT_ILI9163::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
	this->setAddrWindow(x, y, x+w-1, y+h-1);
	this->pushColor(color, w*h);
}

void TFT_ILI9163::drawPixel(uint16_t x, uint16_t y, uint16_t color){
	this->fillRect(x,y,1,1,color);
}

void TFT_ILI9163::drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color){
	this->fillRect(x, y, 1, h, color);
}

void TFT_ILI9163::drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color){
	this->fillRect(x, y, w, 1, color);
}

void TFT_ILI9163::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	this->writeCommand(TFT_CASET);
	this->writeData(x0 >> 8);
	this->writeData(x0 & 0xFF);
	this->writeData(x1 >> 8);
	this->writeData(x1 & 0xFF);

	this->writeCommand(TFT_PASET);
	this->writeData(y0 >> 8);
	this->writeData(y0 & 0xFF);
	this->writeData(y1 >> 8);
	this->writeData(y1 & 0xFF);

	this->writeCommand(TFT_RAMWR);
}

static uint32_t to_rgba888(uint8_t r, uint8_t g, uint8_t b) {
	return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

// Emulates how the display extends 4 and 5 to 6-bit values
// By copying the upper 1 or 2 bits to fill in the lowest significant bits
int TFT_ILI9163::writeRGB(uint32_t* pixels, const uint8_t* buf) {
	int bdiff = 0;
	uint8_t r, g, b;

	switch (this->bpp) {
	case TFT_RGB444:
		r = (buf[0] & 0xF0) | (buf[0] >> 4);
		g = (buf[0] & 0x0F) | ((buf[0] & 0x0F) << 4);
		b = (buf[1] & 0xF0) | (buf[1] >> 4);
		pixels[bdiff++] = to_rgba888(r, g, b);
		
		r = (buf[1] & 0x0F) | ((buf[1] & 0x0F) << 4);
		g = (buf[2] & 0xF0) | (buf[2] >> 4);
		b = (buf[2] & 0x0F) | ((buf[2] & 0x0F) << 4);
		pixels[bdiff++] = to_rgba888(r, g, b);
		break;
	case TFT_RGB565:
		r = (buf[0] & 0xF8) | (buf[0] >> 5);
		g = ((buf[0] & 0x7) << 5) | ((buf[1] & 0xE0) >> 3) | ((buf[0] & 0x7) >> 1);
		b = ((buf[1] & 0x1F) << 3) | ((buf[1] & 0x1F) >> 2);
		pixels[bdiff++] = to_rgba888(r, g, b);
		break;
	case TFT_RGB666:
		pixels[bdiff++] = to_rgba888(buf[0], buf[1], buf[2]);
		break;
	default:
		break;
	}
	
	return bdiff;
}

void TFT_ILI9163::getDims(uint16_t& w, uint16_t& h) const{
	w = this->w;
	h = this->h;
}

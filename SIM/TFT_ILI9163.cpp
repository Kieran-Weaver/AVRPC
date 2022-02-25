#include "TFT_ILI9163.h"
#include <cstddef>

TFT_ILI9163::TFT_ILI9163(){
	this->setSize(128, 160);
}
void TFT_ILI9163::begin(){
	this->refresh();
}
void TFT_ILI9163::setSize(uint16_t w, uint16_t h){
	this->w = w;
	this->h = h;
	this->pixelData.resize(w*h);
}
void TFT_ILI9163::refresh(){
	this->oldPixels.resize(w*h);
	for (auto i = 0; i < this->oldPixels.size(); i++){
		if (this->inverted) {
			this->oldPixels[i] = ~(this->pixelData[i]);
		} else {
			this->oldPixels[i] = this->pixelData[i];
		}
	}
	this->last_count = this->cycle_count;
	this->cycle_count = 0;
	this->oldw = w;
	this->oldh = h;
}
void TFT_ILI9163::writeCommand(ILI9163_COMMANDS command){
	this->command = command;
	this->cycle_count += 2 + this->data_cycles;
	this->cmdnum = 0;
	
	switch (command) {
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
	case TFT_CASET:
		this->addrWindow[0] = 0;
		this->addrWindow[1] = 0;
		this->x  = 0;
		break;
	case TFT_PASET:
		this->addrWindow[2] = 0;
		this->addrWindow[3] = 0;
		this->y  = 0;
		break;
	}
}

void TFT_ILI9163::writeData(uint8_t data){
	this->cycle_count += this->data_cycles;
	size_t curr_x = addrWindow[0] + this->x;
	size_t curr_y = addrWindow[2] + this->y;

	switch (this->command) {
	case TFT_CASET:
		addrWindow[cmdnum / 2] = (addrWindow[cmdnum / 2] << 8) + data;
		break;
	case TFT_PASET:
		addrWindow[cmdnum / 2 + 2] = (addrWindow[cmdnum / 2 + 2] << 8) + data;
		break;
	case TFT_RAMWR:
		pixelData[curr_y * w + curr_x] = (pixelData[curr_y * w + curr_x] << 8) + data;
		if (cmdnum & 1) {
			this->x++;
			if (this->x == addrWindow[1]) {
				this->x = addrWindow[0];
				this->y++;
				if (this->y == addrWindow[3]) {
					this->y = addrWindow[2];
				}
			}
		}
		break;
	}
	
	this->cmdnum++;
}

uint64_t TFT_ILI9163::getCycles() const{
	return this->last_count;
}
const std::vector<uint16_t>& TFT_ILI9163::getPixels() const{
	return this->oldPixels;
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
	this->setAddrWindow(x, y, w, h);
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
void TFT_ILI9163::getDims(uint16_t& w, uint16_t& h) const{
	w = this->w;
	h = this->h;
}

#include "PDQ_ST7735.h"
PDQ_ST7735::PDQ_ST7735(){
	this->setSize(128, 128);
}
void PDQ_ST7735::begin(){
	this->refresh();
}
void PDQ_ST7735::setSize(uint16_t w, uint16_t h){
	this->w = w;
	this->h = h;
	this->pixelData.resize(w*h);
}
void PDQ_ST7735::refresh(){
	this->oldPixels = this->pixelData;
	this->last_count = this->cycle_count;
	this->cycle_count = 0;
	this->oldw = w;
	this->oldh = h;
}
void PDQ_ST7735::writeCommand(uint8_t command){
	this->cycle_count += 2 + this->data_cycles;
}
void PDQ_ST7735::writeData(uint8_t data){
	this->cycle_count += this->data_cycles;
}
uint64_t PDQ_ST7735::getCycles() const{
	return this->last_count;
}
const std::vector<uint16_t>& PDQ_ST7735::getPixels() const{
	return this->oldPixels;
}
void PDQ_ST7735::pushColor(uint16_t color){
	this->writeData(color >> 8);
	this->writeData(color & 0xFF);
	int curr_x = addrWindow[0] + this->x;
	int curr_y = addrWindow[1] + this->y;
	pixelData[curr_y * this->w + curr_x] = color;
	this->x = this->x + 1;
	if (this->x == addrWindow[2]){
		this->x = 0;
		this->y++;
	}
}
void PDQ_ST7735::pushColor(uint16_t color, uint32_t count){
	for (int i = 0; i < count; i++){
		this->pushColor(color);
	}
}
void PDQ_ST7735::fillScreen(uint16_t color){
	this->fillRect(0, 0, w, h, color);
}
void PDQ_ST7735::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
	this->setAddrWindow(x, y, w, h);
	this->pushColor(color, w*h);
}
void PDQ_ST7735::drawPixel(uint16_t x, uint16_t y, uint16_t color){
	this->fillRect(x,y,1,1,color);
}
void PDQ_ST7735::drawFastVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color){
	this->fillRect(x, y, 1, h, color);
}
void PDQ_ST7735::drawFastHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color){
	this->fillRect(x, y, w, 1, color);
}
void PDQ_ST7735::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	this->writeCommand(0x2A); // CASET
	for (int i = 0; i < 4; i++){
		this->writeData(0);
	}
	this->writeCommand(0x2B); // RASET
	for (int i = 0; i < 4; i++){
		this->writeData(0);
	}
	this->writeCommand(0x2C); // RAMWR
	this->addrWindow[0] = x0;
	this->addrWindow[1] = y0;
	this->addrWindow[2] = x1;
	this->addrWindow[3] = y1;
	this->x = 0;
	this->y = 0;
}
void PDQ_ST7735::getDims(uint16_t& w, uint16_t& h) const{
	w = this->w;
	h = this->h;
}

#include "PDQ_ST7735.h"
#include "AVRPC_SIM.h"
uint16_t color565(uint8_t r, uint8_t g, uint8_t b){
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
int main(int argc, char **argv){
	AVRPC avrpc;
	PDQ_ST7735 tft;
	tft.fillScreen(0xFFFF);
	tft.refresh();
	int i = 0;
	while (avrpc){
		tft.fillScreen(i);
		tft.refresh();
		avrpc.Draw(tft);
		i++;
	}
	return 0;
}

#include "TFT_ILI9163.h"
#include "AVRPC_SIM.h"

int main(int argc, char **argv){
	AVRPC avrpc;
	TFT_ILI9163 tft;
	tft.fillScreen(0xFFFF);
	int i = 0;
	uint16_t w, h;
	while (avrpc){
		tft.fillScreen(i);
		tft.getDims(w, h);
		avrpc.Draw(tft.getPixels(), w, h);
		i++;
		if ((i & 255) == 0) {
			tft.writeCommand(TFT_INVOFF);
		}
		if ((i & 511) == 0) {
			tft.writeCommand(TFT_INVON);
		}
	}
	return 0;
}

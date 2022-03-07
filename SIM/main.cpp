#include "TFT_ILI9163.h"
#include "AVRPC_SIM.h"

int main(int argc, char **argv){
	AVRPC avrpc;
	TFT_ILI9163 tft;
	int i = 0;
	uint16_t w, h;
	const auto& initial_state = tft.getState();

	w = initial_state.w;
	h = initial_state.h;

	tft.fillScreen(0xFFFF);
	tft.writeCommand(TFT_VSCRL);
	tft.writeData(0);
	tft.writeData(40);
	tft.writeData(0);
	tft.writeData(80);
	tft.writeData(0);
	tft.writeData(40);

	while (avrpc){
		tft.fillRect(0, 0, w, h/2, i);
		tft.fillRect(0, h/2, w, h/2, 0xFFFF-i);
		
		
		tft.writeCommand(TFT_VSSA);
		tft.writeData(0);
		tft.writeData(i % 160);

		avrpc.Draw(tft.getPixels(), tft.getState());
		i++;
/* Testing for inversion and idle modes
		if ((i & 255) == 0) {
			tft.writeCommand(TFT_INVOFF);
		}
		if ((i & 511) == 0) {
			tft.writeCommand(TFT_INVON);
		}
		if ((i & 63) == 0) {
			tft.writeCommand(TFT_IDLEON);
		}
		if ((i & 128) == 0) {
			tft.writeCommand(TFT_IDLEOFF);
		}
*/ 
	}
	return 0;
}

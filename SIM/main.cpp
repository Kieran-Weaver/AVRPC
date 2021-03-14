#include "TFT_ILI9163.h"
#include "AVRPC_SIM.h"

int main(int argc, char **argv){
	AVRPC avrpc;
	TFT_ILI9163 tft;
	tft.fillScreen(0xFFFF);
	tft.refresh();
	int i = 0;
	while (avrpc){
		tft.fillScreen(i);
		tft.refresh();
		avrpc.Draw(tft);
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

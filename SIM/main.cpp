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
	}
	return 0;
}

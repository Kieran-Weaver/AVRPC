#include "TFT_ILI9163.h"
#include "AVRPC_SIM.h"
#include "avr_core.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../nuklear/nuklear.h"

#include <iostream>

#define F_CPU 16000000

int main(int argc, char **argv){
	AVRPC avrpc;
	TFT_ILI9163 tft;
	bool running = false;
	avr_fw_type fwtype = avr_fw_type::ELF;
	char filename[256] = {0};
	avr_core* avr = new avr_core(
		avr_port{'D', 3}, // Data / CMD
		avr_port{'B', 0}, // Chip Select (active low)
		avr_port{'D', 2}  // Reset
	);
	
	avr->rst() = [&]() {
		tft.fillScreen(0xFF00);
	};
	
	avr->spi() = [&](bool dc, uint8_t byte) {
		if (dc) {
			tft.writeData(byte);
		} else {
			tft.writeCommand(static_cast<ILI9163_COMMANDS>(byte));
		}
	};
	
	int i = 0;
	uint16_t w, h;
	struct nk_context *ctx = avrpc.ctx();
	const auto& initial_state = tft.getState();

	w = initial_state.w;
	h = initial_state.h;

	tft.fillScreen(0xFFFF);

	while (avrpc){		
		if (nk_begin(ctx, "AVRPC Simulator", nk_rect(0, 0, 384, 960),
		NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
			nk_layout_row_dynamic(ctx, 30, 1);
			nk_label(ctx, "Firmware file type: ", NK_TEXT_LEFT);
			if (nk_option_label(ctx, "ELF", fwtype == avr_fw_type::ELF)) fwtype = avr_fw_type::ELF;
			if (nk_option_label(ctx, "HEX", fwtype == avr_fw_type::HEX)) fwtype = avr_fw_type::HEX;
			if (nk_option_label(ctx, "BIN", fwtype == avr_fw_type::BIN)) fwtype = avr_fw_type::BIN;
			nk_label(ctx, "Filename:", NK_TEXT_LEFT);
			nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, filename, sizeof(filename) - 1, nk_filter_default);
			if (nk_button_label(ctx, "Load")) {
				avr->load_program(fwtype, filename);
				running = true;
			}
			nk_end(ctx);
		}

		if (running) {
			avr->run_cycles(F_CPU * (1.f / 60.f));
		}

		avrpc.Draw(tft.getPixels(), tft.getState());
	}
	
	delete avr;
	return 0;
}

#ifndef AVRPC_AVR_CORE_H
#define AVRPC_AVR_CORE_H

#include <functional>
#include <cstdint>
#include <string_view>

using spi_callback = std::function<void(bool, uint8_t)>;
using rst_callback = std::function<void(void)>;
struct avr_t;
struct avr_irq_t;

struct avr_port {
	char port;
	int pin;
};

enum class avr_fw_type {
	ELF,
	HEX,
	BIN
};

struct avr_core {
	avr_core(avr_port dcmd, avr_port csel, avr_port reset);
	~avr_core();
	void load_program(avr_fw_type type, std::string_view filename);
	void run_cycles(unsigned int cycles);
	rst_callback& rst();
	spi_callback& spi();
private:
	bool cs = false; /* Chip Select IRQ never gets called */
	bool dc = true;
	rst_callback rst_cb;
	spi_callback spi_cb;
	avr_t * core;
	avr_irq_t* irqs;
};

#endif

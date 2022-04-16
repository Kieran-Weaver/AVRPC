#include "avr_core.h"

#include <simavr/sim_avr.h>
#include <simavr/sim_elf.h>
#include <simavr/sim_gdb.h>
#include <simavr/sim_hex.h>
#include <simavr/avr_ioport.h>
#include <simavr/avr_spi.h>
#include <fstream>
#include <iosfwd>
#include <iostream>

#define IRQ_COUNT 4
#define IRQ_SPI 0
#define IRQ_RST 1
#define IRQ_DCM 2
#define IRQ_CSL 3

static const char* irq_names[IRQ_COUNT] = {
	[ IRQ_SPI ] = "=avr_core.SPI",
	[ IRQ_RST ] = "<avr_core.RST",
	[ IRQ_DCM ] = "<avr_core.DCM",
	[ IRQ_CSL ] = "<avr_core.CSL"
};

avr_core::avr_core(avr_port dcmd, avr_port csel, avr_port reset) {
	core = avr_make_mcu_by_name("atmega328");
	avr_init(core);

	irqs = avr_alloc_irq(&core->irq_pool, 0, IRQ_COUNT, irq_names);
	
	avr_irq_register_notify(irqs + IRQ_SPI, 
		[](struct avr_irq_t* irq, uint32_t value, void* param) {
			avr_core* avr = static_cast<avr_core*>(param);
			if (!(avr->cs)) {
				avr->spi_cb(avr->dc, value & 0xFF);
			}
		},
		this
	);
	
	avr_irq_register_notify(irqs + IRQ_DCM,
		[](struct avr_irq_t* irq, uint32_t value, void* param) {
			avr_core* avr = static_cast<avr_core*>(param);
			avr->dc = (value & 0xFF) != 0;
		},
		this
	);
	
	avr_irq_register_notify(irqs + IRQ_RST,
		[](struct avr_irq_t* irq, uint32_t value, void* param) {
			avr_core* avr = static_cast<avr_core*>(param);
			if (!(value & 0xFF))
				avr->rst_cb();
		},
		this
	);
	
	avr_irq_register_notify(irqs + IRQ_CSL,
		[](struct avr_irq_t* irq, uint32_t value, void* param) {
			avr_core* avr = static_cast<avr_core*>(param);
			std::cout << value << std::endl;
			avr->cs = (value & 0xFF);
		},
		this
	);
	
	avr_connect_irq(
		avr_io_getirq(core, AVR_IOCTL_SPI_GETIRQ(0), SPI_IRQ_OUTPUT),
		irqs + IRQ_SPI);
	
	avr_connect_irq(
		avr_io_getirq(core,
			AVR_IOCTL_IOPORT_GETIRQ(dcmd.port),
			dcmd.pin),
		irqs + IRQ_DCM);
	
	avr_connect_irq(
		avr_io_getirq(core,
			AVR_IOCTL_IOPORT_GETIRQ(reset.port),
			reset.pin),
		irqs + IRQ_RST);
			
	avr_connect_irq(
		avr_io_getirq(core,
			AVR_IOCTL_IOPORT_GETIRQ(csel.port),
			csel.pin),
		irqs + IRQ_CSL);
}

void avr_core::load_program(avr_fw_type type, std::string_view filename) {
	uint32_t size, base;
	uint8_t * data;
	std::ifstream file;
	elf_firmware_t fw;

	switch (type) {
		case avr_fw_type::ELF:
			elf_read_firmware(filename.data(), &fw);
			avr_load_firmware(core, &fw);
			break;
		case avr_fw_type::HEX:
			data = read_ihex_file(filename.data(), &size, &base);
			avr_loadcode(core, data, size, base);
			free(data);
			break;
		case avr_fw_type::BIN:
			file.open(filename.data(), std::ios::binary | std::ios::ate);
			size = file.tellg();
			file.seekg(0, std::ios::beg);
			data = new uint8_t[size];
			file.read(reinterpret_cast<char*>(data), size);
			avr_loadcode(core, data, size, base);
			delete data;
			break;
	}
}

avr_core::~avr_core() {
	avr_terminate(core);
	free(core);
}

rst_callback& avr_core::rst() {
	return rst_cb;
}

spi_callback& avr_core::spi() {
	return spi_cb;
}

void avr_core::run_cycles(unsigned int cycles) {
	for (unsigned int i = 0; i < cycles; i++) {
		avr_run(core);
	}
}

TARGET  = main
MCU     = atmega328p
F_CPU   = 16000000
PRG     = usbtiny
AVRDUDE = avrdude -c $(PRG) -p $(MCU)
OBJCOPY = avr-objcopy
SIZE    = avr-size
CC      = avr-gcc
CXX     = avr-g++
BOOT_ADDR = 0x7800
LDFLAGS = -Wl,-Map,$(TARGET).map -Wl,--gc-sections -Wl,--section-start,.text=$(BOOT_ADDR)

INC_FLAGS := -I .
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
CFLAGS   = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(INC_FLAGS) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mrelax -flto -DBOOT_ADDR=$(BOOT_ADDR)
CXXFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=c++17   -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mrelax -flto -DBOOT_ADDR=$(BOOT_ADDR)

SRCS=avrboot.cpp $(shell find pff -path "*.cpp") $(shell find . -path "*.S")
#SRCS=SDTest.cpp $(shell find pff -path "*.cpp") $(shell find . -path "*.S")
OBJS=$(patsubst %.S, ./build/%.o, $(patsubst %.cpp, ./build/%.o, $(SRCS)))
DEPS := $(OBJS:.o=.d)

all: $(TARGET).hex

test:
	$(AVRDUDE) -v

flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i

clean:
	rm -rf *.hex *.elf build/

./build/%.o : ./%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

./build/%.o: ./%.S
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $< -flto

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET).elf $(OBJS)

$(TARGET).hex: $(TARGET).elf
	rm -f $(TARGET).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).elf

-include $(DEPS)

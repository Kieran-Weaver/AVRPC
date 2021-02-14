TARGET  = main
MCU     = atmega328p
F_CPU   = 16000000
PRG     = arduino -P /dev/ttyACM0
AVRDUDE = avrdude -c $(PRG) -p $(MCU)
OBJCOPY = avr-objcopy
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-gcc
CXX     = avr-g++

INC_FLAGS := -I .
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
CFLAGS   = -Wall -Os -g -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(INC_FLAGS) -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mrelax
CXXFLAGS = -Wall -Os -g -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=c++17   -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mrelax

SRCS=$(shell find . -path "*.cpp") $(shell find . -path "*.S")
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
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET).elf $(OBJS)

$(TARGET).hex: $(TARGET).elf
	rm -f $(TARGET).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).elf

-include $(DEPS)

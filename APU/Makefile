TARGET  = apu
MCU     = attiny85
F_CPU   = 16000000
PRG     = usbtiny
AVRDUDE = avrdude -c $(PRG) -p $(MCU)
OBJCOPY = avr-objcopy
SIZE    = avr-size
CC      = avr-gcc
CXX     = avr-g++

INC_FLAGS := -I .
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
AVR_FLAGS = 
CFLAGS   = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(AVR_FLAGS) $(INC_FLAGS) -fpack-struct -fshort-enums -mrelax
CXXFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(AVR_FLAGS) -std=c++17 -fpack-struct -fshort-enums -mrelax

SRCS=$(shell find . -path "*.cpp") $(shell find . -path "*.S")
OBJS=$(patsubst %.S, ./build/%.o, $(patsubst %.cpp, ./build/%.o, $(SRCS)))
DEPS := $(OBJS:.o=.d)

all: $(TARGET).hex

test:
	$(AVRDUDE) -v

flash: $(TARGET).hex
	sudo $(AVRDUDE) -U flash:w:$(TARGET).hex:i

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

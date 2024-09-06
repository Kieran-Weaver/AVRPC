TARGET  = sim
CXX     = g++
INC_FLAGS :=
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
CXXFLAGS = -Wno-narrowing -O2 -std=c++17
EXE :=
SRCS=$(shell find . -path "*.cpp")
OBJS=$(patsubst %.cpp, ./build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)
LDFLAGS :=

ifeq ($(OS),Windows_NT)
	EXE += .exe
	LDFLAGS += -mconsole -lwinmm -lgdi32
else
	INC_FLAGS += $(shell sdl2-config --cflags)
	LDFLAGS += $(shell sdl2-config --libs)
endif

all: $(TARGET)$(EXE)

clean:
	rm -rf $(TARGET)$(EXE) build/

./build/%.o : ./%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGET)$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)$(EXE)

-include $(DEPS)

CXX     = g++
INC_FLAGS := $(shell sdl2-config --cflags)
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
CXXFLAGS = -Wno-narrowing -O2 -std=c++17
LDFLAGS := $(shell sdl2-config --libs)

SRCS=$(shell find sim -path "*.cpp")
OBJS=$(patsubst %.cpp, ./build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

TARGET_C = $(wildcard *.cpp)
TARGET_O = $(patsubst %.cpp, ./build/%.o, $(TARGET_C))
TARGET_D = $(TARGET_O:.o=.d)
TARGETS  = $(patsubst %.cpp, %, $(TARGET_C))

all: $(TARGETS)

clean:
	rm -rf $(TARGETS) build/

./build/%.o : ./%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGETS): $(OBJS) $(TARGET_O)
	$(CXX) $(CXXFLAGS) $(OBJS) ./build/$@.o $(LDFLAGS) -o $@

-include $(DEPS)
-include $(TARGET_D)

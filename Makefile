TARGET  = sim
CXX     = g++
INC_FLAGS := $(shell sdl2-config --cflags)
CPPFLAGS = $(INC_FLAGS) -MT $@ -MMD -MP -MF build/$*.d
CXXFLAGS = -Wno-narrowing -O2 -std=c++17
SRCS=$(shell find . -path "*.cpp")
OBJS=$(patsubst %.cpp, ./build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)
LDFLAGS := $(shell sdl2-config --libs)


all: $(TARGET)

clean:
	rm -rf $(TARGET) build/

./build/%.o : ./%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

-include $(DEPS)

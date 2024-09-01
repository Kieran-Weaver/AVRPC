CXX := g++
EXE := main
SRCS := $(shell find . -name '*.cpp')
CXXFLAGS := -Wno-narrowing -Og -g $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs)

all: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(EXE)

clean:
	rm -f $(EXE)

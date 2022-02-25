CC=gcc
CXX=g++
TARGET=Sim
CFLAGS := -Og -g -march=native
CXXFLAGS := -Og -g -march=native -fno-rtti -std=c++17
SRCS := $(shell find . -path "*.cpp") $(shell find . -path "*.c")
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRCS)))

ifdef OS
	LDFLAGS=-Wl,-O1 -static-libstdc++ -static-libgcc -static -lz
	GL_FLAGS=-lglbinding -lopengl32 -lglfw3 -mwindows
else
	LDFLAGS=-lz -g -Wl,-O1
	GL_FLAGS=-lglbinding -lGL -lglfw
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $(TARGET) $(GL_FLAGS) $(LDFLAGS)


%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o : %.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -fr $(TARGET) $(OBJS)

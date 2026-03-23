CXX = clang++

# build script for MACOS

CXXFLAGS = -g -std=c++20 -arch arm64 \
		   -D__MACOSX_CORE__ \
		   $(shell pkg-config --cflags raylib)

LDFLAGS = -framework CoreMIDI \
		  -framework CoreAudio \
		  -framework CoreFoundation \
		  $(shell pkg-config --libs raylib)

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/synth

EXTERNAL_DIRS = external/kissfft external/miniaudio external/rtmidi external/readerwriterqueue

CXXFLAGS += $(addprefix -I, $(EXTERNAL_DIRS))


SRCS = $(shell find $(SRC_DIR) -name "*.cpp") \
	   external/kissfft/kiss_fft.c \
	   external/kissfft/kiss_fftr.c \
	   external/rtmidi/RtMidi.cpp

# OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS))) \
       $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/$(SRC_DIR)

clean-all:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run


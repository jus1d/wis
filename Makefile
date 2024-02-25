CXX ?= g++
CXXFLAGS ?= -std=c++23 -Wall -Wextra
TARGET ?= gollo
SOURCE ?= main.cpp

BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE), debug)
    CXXFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET)

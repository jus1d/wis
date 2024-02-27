CXX ?= g++
CXXFLAGS ?= -std=c++23# -Wall -Wextra
TARGET ?= gollo test
SOURCE ?= gollo.cpp test.cpp

BUILD_TYPE ?= release

ifeq ($(BUILD_TYPE), debug)
    CXXFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $<
	$(CXX) $(CXXFLAGS) -o ./test ./test.cpp

.PHONY: clean

clean:
	rm -f $(TARGET)

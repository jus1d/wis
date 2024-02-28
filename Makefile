CXX ?= g++
CXXFLAGS ?= -std=c++23# -Wall -Wextra

all: build test

build: ./gollo.cpp
	$(CXX) $(CXXFLAGS) -o ./gollo ./gollo.cpp

test: ./test.cpp
	$(CXX) $(CXXFLAGS) -o ./test ./test.cpp
	./test run -f ./euler/ -f ./examples/

clean:
	rm -f ./test ./gollo

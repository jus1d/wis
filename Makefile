CXX ?= g++
CXXFLAGS ?= -std=c++23# -Wall -Wextra

build: ./gollo.cpp ./test.cpp
	$(CXX) $(CXXFLAGS) -o ./gollo ./gollo.cpp
	$(CXX) $(CXXFLAGS) -o ./test ./test.cpp

test: build
	./test run -f ./examples/ -f ./euler/
	

clean:
	rm -f ./test ./gollo

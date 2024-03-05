CXX ?= g++
CXXFLAGS ?= -std=c++23# -Wall -Wextra

build: ./wis.cpp ./test.cpp
	$(CXX) $(CXXFLAGS) -o ./wis ./wis.cpp
	$(CXX) $(CXXFLAGS) -o ./test ./test.cpp

test: build
	./test run -f ./examples/
	

clean:
	rm -f ./test ./wis

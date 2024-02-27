cmake_minimum_required(VERSION 3.27)
project(gollo)

set(CMAKE_CXX_STANDARD 23)

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_executable(gollo gollo.cpp
        assert.h)

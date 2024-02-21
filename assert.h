#include <iostream>

#ifndef ASSERT_H
#define ASSERT_H

void assertion_error(const char* file, int line, const char* function) {
    std::cerr << file << ":" << line << ": Assertion failed in " << function << "()" << std::endl;
}

#define assert(condition, message) \
    do { \
        if (!(condition)) {        \
            assertion_error(__FILE__, __LINE__, __func__); \
            cerr << message << endl; \
            exit(1); \
        } \
    } while (false)

#endif
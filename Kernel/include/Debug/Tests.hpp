#ifndef __TESTS_HPP
#define __TESTS_HPP

#define EXPECT(value, expectedValue) return expect(value, expectedValue, __FUNCTION__, __FILE__, __LINE__)

int expect(int value, int expectedValue, const char* function, char* file, int line);
int expect(char* value, char* expectedValue, const char* function, char* file, int line);

void performTests();

#endif
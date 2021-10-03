#ifndef __STRING_HPP
#define __STRING_HPP

#include <Lib/Types.hpp>

u32 strlen(const char* str);
void* memset(void* dest, char val, u32 count);
void memcpy(void* dest, void* src, u32 size);
int strcmp(const char* a, const char* b);

#endif

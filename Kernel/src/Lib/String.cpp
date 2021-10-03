#include <Lib/String.hpp>

u32 strlen(const char* str) {
    u32 len = 0;

    while (str[len]) {
        len++;
    }

    return len;
}

void* memset(void* dest, char val, u32 count) {
    u8* temp = (u8*) dest;
    for (; count != 0; count--) {
        temp[count] = val;
    }

    return dest;
}

void memcpy(void* dest, void* src, u32 size) {
    char* csrc = (char*) src;
    char* cdest = (char*) dest;

    for (u32 i = 0; i < size; i++) {
        cdest[i] = csrc[i];
    }
}

int strcmp(const char* a, const char* b) {
    while (*a) {
        if (*a != *b) {
            break;
        }

        a++;
        b++;
    }

    return *(const unsigned char*) a - *(const unsigned char*) b;
}
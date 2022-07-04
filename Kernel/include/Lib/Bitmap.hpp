#ifndef __BITMAP_HPP
#define __BITMAP_HPP

#include "Types.hpp"

class Bitmap {
public:
    Bitmap(u32* memoryMap, u32 size, bool initializeWithOne);

    void set(u32 bit);
    void unset(u32 bit);
    bool test(u32 bit);

    u32 getFirstFree(u32 count = 1);

private:
    u32* memoryMap;
    u32 size;
};

#endif
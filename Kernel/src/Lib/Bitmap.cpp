#include <Lib/Bitmap.hpp>
#include <Lib/String.hpp>

Bitmap::Bitmap(u32* memoryMap, u32 size, bool initializeWithOne) {
    if (initializeWithOne) {
        memset(memoryMap, 0xFF, size);
    } else {
        memset(memoryMap, 0, size);
    }

    this->memoryMap = memoryMap;
    this->size = size;
}

void Bitmap::set(u32 bit) {
    memoryMap[bit / 32] |= (1 << (bit % 32));
}

void Bitmap::unset(u32 bit) {
    memoryMap[bit / 32] &= ~(1 << (bit % 32));
}

u32 Bitmap::getFirstFree(u32 count) {
    if (count == 0) {
        return -1;
    }

    for (u32 i = 0; i < size; i++) {
        if (memoryMap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                if (!(memoryMap[i] & (1 << j))) {
                    int currentBit = i * 32 + j;

                    u32 free = 0;
                    for (u32 k = 0; k < count; k++) {
                        if (!test(currentBit + k)) {
                            free++;
                        }

                        if (free == count) {
                            return currentBit;
                        }
                    }
                }
            }
        }
    }

    return -1;
}

bool Bitmap::test(u32 bit) {
    return memoryMap[bit / 32] & (1 << (bit % 32));
}

#ifndef __HEAP_HPP
#define __HEAP_HPP

#include <Lib/Types.hpp>

struct MemoryChunk {
    MemoryChunk* next;
    MemoryChunk* prev;

    bool allocated;
    u32 size;
};

class Heap {
public:
    Heap(u32 start, u32 size);

    void* malloc(u32 size);

    void free(void* ptr);

private:
    u32 memorySize;
    u32 usedMemory;

    MemoryChunk* first;
};

#endif
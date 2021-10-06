#ifndef __KMALLOC_HPP
#define __KMALLOC_HPP

#include "Heap.hpp"

class BasicMemoryAllocator : public Singleton<BasicMemoryAllocator> {
public:
    BasicMemoryAllocator();

    void* malloc(u32 size);
    void free(void* address);

private:
    Heap heap;
};

void* kmalloc(u32 size);
void free(void* address);

#endif
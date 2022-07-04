#ifndef __KMALLOC_HPP
#define __KMALLOC_HPP

#include <MM/Heap.hpp>
#include <Lib/Singleton.hpp>

#define KHEAP_START 0xC0000000 + 8 * 1024 * 1024
#define KHEAP_SIZE 2 * 1024 * 1024

class BasicMemoryAllocator : public Singleton<BasicMemoryAllocator> {
public:
    BasicMemoryAllocator();

    void* malloc(u32 size);
    void free(void* address);

    u32 usage();

//private:
    Heap heap;
};

void* kmalloc(u32 size);
void free(void* address);

#endif
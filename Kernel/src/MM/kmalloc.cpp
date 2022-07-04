#include <Lib/Log.hpp>
#include <MM/kmalloc.hpp>

// basic heap before initializing virtual memory
// I don't know if it's right to do this
// but it works so :shrug:

static u8 kmallocHeap[1024];

BasicMemoryAllocator::BasicMemoryAllocator() : heap((u32) kmallocHeap, 1024) {
    instance = this;
}

void* BasicMemoryAllocator::malloc(u32 size) {
    return heap.malloc(size);
}

void BasicMemoryAllocator::free(void* address) {
    return heap.free(address);
}

u32 BasicMemoryAllocator::usage() {
    return heap.usage();
}

void* kmalloc(u32 size) {
    return BasicMemoryAllocator::instance->malloc(size);
}

void free(void* address) {
    return BasicMemoryAllocator::instance->free(address);
}

void* operator new(long unsigned int size) {
    return kmalloc(size);
}

void* operator new[](long unsigned int size) {
    return kmalloc(size);
}

void* operator new(long unsigned int size, void* ptr) {
    return ptr;
}

void* operator new[](long unsigned int size, void* ptr) {
    return ptr;
}

void operator delete(void* ptr) {
    free(ptr);
}

void operator delete(void* ptr, u32 size) {
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr, u32 size) {
    free(ptr);
}


#include <Lib/Log.hpp>
#include <MM/kmalloc.hpp>

// 4MB for kernel heap are mapped in the initial boot process
// I don't know if it's right to do this
// but it works well so :shrug:

BasicMemoryAllocator::BasicMemoryAllocator() : heap((u32) KHEAP_START, 4 * 1024 * 1024) {
    instance = this;
}

void* BasicMemoryAllocator::malloc(u32 size) {
    return heap.malloc(size);
}

void BasicMemoryAllocator::free(void* address) {
    return heap.free(address);
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


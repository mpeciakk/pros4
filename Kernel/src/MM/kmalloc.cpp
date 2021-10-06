#include <Lib/Log.hpp>
#include <MM/kmalloc.hpp>

// It's a bad implementation of kernel heap
// but unless we initialize memory manager it's enough I think
__attribute__((section(".heap"))) static u8 kmallocHeap[2 * 1024 * 1024];

BasicMemoryAllocator::BasicMemoryAllocator() : heap((u32) kmallocHeap, 2 * 1024 * 1024) {
    instance = this;

//    klog(0, "0x%x", (u32) kmallocHeap);
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


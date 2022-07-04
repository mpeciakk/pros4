#include <Lib/Log.hpp>
#include <Lib/String.hpp>
#include <MM/Heap.hpp>

Heap::Heap(u32 start, u32 size) {
    memset((void*) start, 0, size);

    usedMemory = 0;
    memorySize = size;

    if (size < sizeof(MemoryChunk)) {
        first = nullptr;
    } else {
        first = (MemoryChunk*) start;

        first->allocated = false;
        first->prev = nullptr;
        first->next = nullptr;
        first->size = size - sizeof(MemoryChunk);

        first->magic = HEAP_MAGIC;
    }
}

u32 Heap::usage() {
    return usedMemory;
}

void* Heap::malloc(u32 size) {
    if (usedMemory + size + sizeof(MemoryChunk) >= this->memorySize) {
        klog(2, "HEAP: Out of memory [usedMemory + memorySize is bigger than total memorySize]");

        return nullptr;
    }

    MemoryChunk* result = nullptr;

    for (MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next) {
        if (chunk->magic != HEAP_MAGIC) {
            klog(1, "HEAP: MemoryChunk magic number is incorrect");
        }

        if (chunk->size > size && !chunk->allocated) {
            result = chunk;
        }
    }

    if (result == nullptr) {
        klog(2, "HEAP: Out of memory [can't find free memory chunk]");

        return nullptr;
    }

    if (result->size >= size + sizeof(MemoryChunk)) {
        auto* temp = (MemoryChunk*) ((u32) result + sizeof(MemoryChunk) + size);
        memset(temp, 0, sizeof(MemoryChunk));

        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = nullptr;
        temp->magic = HEAP_MAGIC;

        if (temp->next != nullptr) {
            temp->next->prev = temp;
        }

        result->size = size;
        result->next = temp;
        result->allocated = true;
    }

    usedMemory += size + sizeof(MemoryChunk);

    return (void*) (((u32) result) + sizeof(MemoryChunk));
}

void Heap::free(void* ptr) {
    auto* chunk = (MemoryChunk*) ((u32) ptr - sizeof(MemoryChunk));

    chunk->allocated = false;

    if (chunk->prev != nullptr && chunk->prev->allocated) {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);

        if (chunk->next != nullptr) {
            chunk->next->prev = chunk->prev;
        }

        chunk = chunk->prev;
    }

    if (chunk->next != nullptr && !chunk->next->allocated) {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;

        if (chunk->next != nullptr) {
            chunk->next->prev = chunk->prev;
        }
    }
}

void Heap::dump() {
    MemoryChunk* tmp;

    tmp = first;

    while (tmp != nullptr) {
        klog(0, "\nprev - %d\ncurr - %d\nnext - %d\nsize - %d\nallocated - %d\nmagic - %d", tmp->prev, tmp, tmp->next, tmp->size, tmp->allocated, tmp->magic);

        tmp = tmp->next;
    }
}

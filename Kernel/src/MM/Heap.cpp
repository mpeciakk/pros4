#include <Lib/Log.hpp>
#include <MM/Heap.hpp>

Heap::Heap(u32 start, u32 size) {
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
    }
}

void* Heap::malloc(u32 size) {
    if (usedMemory + size + sizeof(MemoryChunk) >= this->memorySize) {
        klog(2, "HEAP: Out of memory [usedMemory + memorySize is bigger than total memorySize]");

        return nullptr;
    }

    MemoryChunk* result = nullptr;

    for (MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next) {
        if (chunk->size > size && !chunk->allocated) {
            result = chunk;
        }
    }

    if (result == nullptr) {
        klog(2, "Out of memory [result is null]");

        return nullptr;
    }

    if (result->size >= size + sizeof(MemoryChunk) + 1) {
        auto* temp = (MemoryChunk*) ((u32) result + sizeof(MemoryChunk) + size);
        memset(temp, 0, sizeof(MemoryChunk));

        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;

        if (temp->next != nullptr) {
            temp->next->prev = temp;
        }

        result->size = size;
        result->next = temp;
    }

    result->allocated = true;

    usedMemory += size + sizeof(MemoryChunk);

    for (MemoryChunk* chunk = first; chunk != nullptr; chunk = chunk->next) {
        if (chunk->size > size && !chunk->allocated) {
            result = chunk;
        }
    }


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
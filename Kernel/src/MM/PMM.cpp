#include <Lib/Log.hpp>
#include <MM/Address.hpp>
#include <MM/PMM.hpp>
#include <Multiboot.hpp>

#define phys2virt(x) ((u32) (x) + (u32) 3 * 1024 * 1024 * 1024)

PhysicalMemoryManager::PhysicalMemoryManager() {
    instance = this;
}

void PhysicalMemoryManager::parseMemoryMap(u32* memoryMapAddress, u32 memoryMapSize) {
    auto* mmapStart = (multiboot_mmap_entry*) phys2virt(memoryMapAddress);

    for (auto* mmap = mmapStart; (u32) mmap < (phys2virt(memoryMapAddress) + memoryMapSize);
         mmap = (multiboot_mmap_entry*) ((u32) mmap + mmap->size + sizeof(mmap->size))) {
        klog(0, "MM: Address low = %d, address high = %x, length low = %x, length high = %x, type = %x", mmap->addr_low, mmap->addr_high,
             mmap->len_low, mmap->len_high, mmap->type);
    }
}
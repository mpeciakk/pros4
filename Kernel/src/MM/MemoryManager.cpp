#include <Lib/Log.hpp>
#include <Lib/Vector.hpp>
#include <MM/Address.hpp>
#include <MM/MemoryManager.hpp>
#include <Multiboot.hpp>

#define phys2virt(x) ((u32) (x) + (u32) 3 * 1024 * 1024 * 1024)
#define virt2phys(x) ((u32) (x) - (u32) 3 * 1024 * 1024 * 1024)

extern "C" u32* kernelStart;
extern "C" u32* kernelEnd;

MemoryManager::MemoryManager() {
    instance = this;
}

void MemoryManager::parseMemoryMap(u32* memoryMapAddress, u32 memoryMapSize) {
    auto* mmapStart = (multiboot_mmap_entry*) phys2virt(memoryMapAddress);

    u32 kernelSize = (u32) &kernelEnd - (u32) &kernelStart;

    Vector<PhysicalMemoryRange> memory;

    u32 memorySize = 0;

    for (auto* mmap = mmapStart; (u32) mmap < (phys2virt(memoryMapAddress) + memoryMapSize);
         mmap = (multiboot_mmap_entry*) ((u32) mmap + mmap->size + sizeof(mmap->size))) {

        // TODO: add low and high together
        auto address = PhysicalAddress(mmap->addr_low);
        auto length = mmap->len_low;

        memorySize += length;

        switch (mmap->type) {
            case (MULTIBOOT_MEMORY_AVAILABLE):
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::Usable, address, length });
                break;
            case (MULTIBOOT_MEMORY_RESERVED):
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::Reserved, address, length });
                break;
            case (MULTIBOOT_MEMORY_ACPI_RECLAIMABLE):
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::ACPI_Reclaimable, address, length });
                break;
            case (MULTIBOOT_MEMORY_NVS):
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::ACPI_NVS, address, length });
                break;
            case (MULTIBOOT_MEMORY_BADRAM):
                klog(1, "MM: Detected bad memory range!");
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::BadMemory, address, length });
                break;
            default:
                klog(1, "MM: Unknown range!");
                memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::Unknown, address, length });
                break;
        }
    }

    memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::Used, PhysicalAddress(0x00000000), 1 * 1024 * 1024 });
    memory.append(PhysicalMemoryRange { PhysicalMemoryRangeType::Used, PhysicalAddress(virt2phys(&kernelStart)), kernelSize });

    klog(0, "MM: MemorySize: %d KB", memorySize / 1024);

    physicalMemoryMap = new Bitmap((u32*) 0xC0300000, memorySize / 4096 / 32, true);

    for (PhysicalMemoryRange* range = memory.begin(); range < memory.end(); range++) {
        klog(0, "MM: Type: %d, Address: 0x%x, Length: 0x%x", range->type, range->start, range->length);

        auto address = range->start.get();
        auto length = range->length;

        for (u32 pageBase = address; pageBase <= (address + length); pageBase += PAGE_SIZE) {
            physicalMemoryMap->unset(pageBase / PAGE_SIZE);

            if (range->type == PhysicalMemoryRangeType::Usable) {
                physicalMemoryMap->unset(pageBase / PAGE_SIZE);
            } else {
                physicalMemoryMap->set(pageBase / PAGE_SIZE);
            }
        }
    }

    klog(0, "MM: First free physical page: 0x%x", getFreePhysicalPage());
}

PhysicalAddress* MemoryManager::getFreePhysicalPage(u32 size) {
    return new PhysicalAddress(physicalMemoryMap->getFirstFree(size) * 4096);
}

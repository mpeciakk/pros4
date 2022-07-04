#include <Lib/Log.hpp>
#include <Lib/Vector.hpp>
#include <MM/MemoryManager.hpp>
#include <Multiboot.hpp>

#define phys2virt(x) ((u32) (x) + (u32) 3 * 1024 * 1024 * 1024)
#define virt2phys(x) ((u32) (x) - (u32) 3 * 1024 * 1024 * 1024)

extern "C" u32* kernelStart;
extern "C" u32* kernelEnd;

struct PageTableHolder {
    u32* virtualAddress;
    u32 physicalAddress;
};

struct PageDirectoryHolder {
    u32* virtualAddress;
    u32 physicalAddress;
    PageTableHolder tables[1024];
};

PageDirectoryHolder kernelPageDirectory;
PageDirectoryHolder* currentPageDirectory;

MemoryManager::MemoryManager() {
    instance = this;
}

void MemoryManager::parseMemoryMap(u32* memoryMapAddress, u32 memoryMapSize) {
    auto* memoryMapStart = (multiboot_mmap_entry*) phys2virt(memoryMapAddress);

    u32 kernelSize = (u32) &kernelEnd - (u32) &kernelStart;

    Vector<PhysicalMemoryRange> memory;

    u32 memorySize = 0;

    for (auto* entry = memoryMapStart; (u32) entry < (phys2virt(memoryMapAddress) + memoryMapSize);
         entry = (multiboot_mmap_entry*) ((u32) entry + entry->size + sizeof(entry->size))) {

        // TODO: add low and high together
        auto address = PhysicalAddress(entry->addr_low);
        auto length = entry->len_low;

        memorySize += length;

        switch (entry->type) {
            case (MULTIBOOT_MEMORY_AVAILABLE):
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Usable, address, length});
                break;
            case (MULTIBOOT_MEMORY_RESERVED):
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Reserved, address, length});
                break;
            case (MULTIBOOT_MEMORY_ACPI_RECLAIMABLE):
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::ACPI_Reclaimable, address, length});
                break;
            case (MULTIBOOT_MEMORY_NVS):
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::ACPI_NVS, address, length});
                break;
            case (MULTIBOOT_MEMORY_BADRAM):
                klog(1, "MM: Detected bad memory range!");
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::BadMemory, address, length});
                break;
            default:
                klog(1, "MM: Unknown range!");
                memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Unknown, address, length});
                break;
        }
    }

    memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Used, PhysicalAddress(0x00000000), 1 * 1024 * 1024});
    memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Used, PhysicalAddress(virt2phys(&kernelStart)), kernelSize});

    klog(0, "MM: MemorySize: %d KB", memorySize / 1024);

    physicalMemoryMap = new Bitmap((u32*) 0xC0300000, memorySize / 1024 / 4 / 32, true);

    memory.append(PhysicalMemoryRange{PhysicalMemoryRangeType::Used, PhysicalAddress(0xC0300000), memorySize / 1024 / 4 / 32});

    for (PhysicalMemoryRange* range = memory.begin(); range < memory.end(); range++) {
        klog(0, "MM: Type: %d, Address: 0x%x, Length: 0x%x", range->type, range->start, range->length);

        auto address = range->start.get();
        auto length = range->length;

        if (range->type == PhysicalMemoryRangeType::Usable) {
            klog(3, "%d", range->start);
        }

        for (u32 pageBase = address; pageBase <= (address + length); pageBase += PAGE_SIZE) {
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
    return new PhysicalAddress(physicalMemoryMap->getFirstFree(size) * PAGE_SIZE);
}

void MemoryManager::freePhysicalPage(void* address) {
    physicalMemoryMap->unset((u32) address / PAGE_SIZE);
}

void MemoryManager::createEntryPage(u32 address, bool write, bool user) {
    u32 physicalAddress = (u32) getFreePhysicalPage();
    address &= ~(0xFFF); // sanitize the address, and make sure it's page aligned
    physicalAddress &= ~(0xFFF);

    u32 pageDirIndex = address >> 22;
    u32 pageTableIndex = (address >> 12) & 0x3FF;

    auto* directory = (PageDirectory*) currentPageDirectory->virtualAddress;

    if (directory->entries[pageDirIndex].present == 0) {
        // create a page table

        u32 tablePhysicalAddress = (u32) getFreePhysicalPage();

        // make sure that's not the page we're trying to map
        if (tablePhysicalAddress == physicalAddress) {
            u32 temp = tablePhysicalAddress;
            tablePhysicalAddress = (u32) getFreePhysicalPage();
            freePhysicalPage((void*) temp);
        }

        // psycha siada

        u32* kernelPageTable = currentPageDirectory->tables[768].virtualAddress;

        u32 kernelPT10 = kernelPageTable[10];

        // map the new page to 0xC000A000
        kernelPageTable[10] = tablePhysicalAddress | 3;

        // create a pointer ot the new page so i can alter it
        u32* pageTable = (u32*) 0xC000A000;
        memset(pageTable, 0, 4096);

        // create an empty page table in there.
        for (u32 i = 0; i < 1024; i++) {
            pageTable[i] = 0 | 2;
        }

        // put the physical address of the new page table on the page directory at the proper index
        PageDirectoryEntry entry;
        memset(&entry, 0, sizeof(PageDirectoryEntry));
        *((u32*) &entry) = tablePhysicalAddress | 3;
        directory->entries[pageDirIndex] = entry;

        // figure out the recursive address for the new page table
        u32 page_table_recursive_addr = 0xFFC00000 + (0x1000 * pageDirIndex);

        // populate the proper values on the data structure
        currentPageDirectory->tables[pageDirIndex].virtualAddress = (u32*) page_table_recursive_addr;
        currentPageDirectory->tables[pageDirIndex].physicalAddress = tablePhysicalAddress;

        // restore the original mapping on PT10
        kernelPageTable[10] = kernelPT10;
    }

    // get the attributes for the page table
    u32 table_attribs = currentPageDirectory->virtualAddress[pageDirIndex] & 0xFFF;

    // create a pointer to the page table so i can alter it
    u32* page_table = currentPageDirectory->tables[pageDirIndex].virtualAddress;

    // map the physical address
    page_table[pageTableIndex] = physicalAddress | table_attribs;

    asm volatile("invlpg (%0)" : : "b"(address) : "memory");
}

void MemoryManager::initPaging() {
    // TODO: change this to calculated value
    u32 pageDirectoryVirtualAddress = 0xC0000000 + 3 * 1024 * 1024 + 1024;

    if (pageDirectoryVirtualAddress % 0x1000 != 0) {
        pageDirectoryVirtualAddress = (pageDirectoryVirtualAddress & ~(0xFFF)) + 0x1000;
    }

    u32 pageTableVirtualAddress = pageDirectoryVirtualAddress + 0x1000;

    u32 pageDirectoryPhysicalAddress = pageDirectoryVirtualAddress - 0xC0000000;
    u32 pageTablePhysicalAddress = pageTableVirtualAddress - 0xC0000000;

    auto* pageDirectory = (PageDirectory*) pageDirectoryVirtualAddress;
    memset(pageDirectory, 0, 4096);

    for (u32 i = 0; i < 1024; i++) {
        PageDirectoryEntry entry;
        entry.readWrite = 1;

        pageDirectory->entries[i] = entry;
    }

    auto* pageTable = (PageTable*) pageTableVirtualAddress;
    memset(pageTable, 0, 4096);

    for (u32 i = 0; i < 1024; i++) {
        PageTableEntry entry;
        memset(&entry, 0, sizeof(PageTableEntry));
        entry.present = 1;
        entry.readWrite = 1;
        entry.frame = i;

        pageTable->entries[i] = entry;
    }

    u32 kernelIndex = 0xC0000000 >> 22;

    // Map first 4MB at 0xC0000000 as present and writable
    PageDirectoryEntry entry;
    memset(&entry, 0, sizeof(PageDirectoryEntry));
    *((u32*) &entry) = pageTablePhysicalAddress | 3;
    pageDirectory->entries[kernelIndex] = entry;

    // Setup recursive page table and page directory
    PageDirectoryEntry entry2;
    memset(&entry2, 0, sizeof(PageDirectoryEntry));
    *((u32*) &entry2) = pageDirectoryPhysicalAddress | 3;
    pageDirectory->entries[1023] = entry2;

    asm("movl %0, %%cr3" : : "r"(pageDirectoryPhysicalAddress));

    kernelPageDirectory.virtualAddress = (u32*) pageDirectoryVirtualAddress;
    kernelPageDirectory.physicalAddress = pageDirectoryPhysicalAddress;

    for (u32 i = 0; i < 1024; i++) {
        kernelPageDirectory.tables[i].virtualAddress = 0;
        kernelPageDirectory.tables[i].physicalAddress = 0;
    }

    kernelPageDirectory.tables[kernelIndex].virtualAddress = (u32*) 0xFFF00000;
    kernelPageDirectory.tables[kernelIndex].physicalAddress = pageTablePhysicalAddress;

    kernelPageDirectory.tables[1023].virtualAddress = (u32*) 0xFFC00000;
    kernelPageDirectory.tables[1023].physicalAddress = pageDirectoryPhysicalAddress;

    currentPageDirectory = &kernelPageDirectory;

    for (u32 i = KHEAP_START; i < KHEAP_START + KHEAP_SIZE; i += PAGE_SIZE) {
        createEntryPage(i, true, false);
    }
}

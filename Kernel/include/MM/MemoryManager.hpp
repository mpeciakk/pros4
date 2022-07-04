#ifndef __MEMORYMANAGER_HPP
#define __MEMORYMANAGER_HPP

#include <Lib/Bitmap.hpp>
#include <Lib/Singleton.hpp>
#include <Lib/Types.hpp>
#include <MM/Address.hpp>

#define PAGE_SIZE 4096

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR 1024

#define PAGE_DIRECTORY_INDEX(x) ((u32) x) >> 22
#define PAGE_TABLE_INDEX(x) (((u32) x) >> 12) & 0x3ff

enum class PhysicalMemoryRangeType {
    Usable = 0,
    Reserved,
    ACPI_Reclaimable,
    ACPI_NVS,
    BadMemory,
    Used,
    Unknown,
};

struct PhysicalMemoryRange {
    PhysicalMemoryRangeType type { PhysicalMemoryRangeType::Unknown };
    PhysicalAddress start {};
    u32 length {};
};

struct PageDirectoryEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 reserved : 1;
    u32 pageSize : 1;
    u32 ignored : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTableEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 dirty : 1;
    u32 reserved : 1;
    u32 global : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTable {
    PageTableEntry entries[PAGES_PER_TABLE];

    inline PageTableEntry* lookupEntry(u32 address) {
        return &entries[PAGE_TABLE_INDEX(address)];
    }
};

struct PageDirectory {
    PageDirectoryEntry entries[PAGES_PER_DIR];
};

class MemoryManager : public Singleton<MemoryManager> {
public:
    MemoryManager();

    void parseMemoryMap(u32* memoryMap, u32 memoryMapSize);
    void initPaging();
    PhysicalAddress* getFreePhysicalPage(u32 size = 1);
    void freePhysicalPage(void* address);

    void createEntryPage(u32 address, bool write, bool user);

private:
    Bitmap* physicalMemoryMap;
};

#endif
#ifndef __MEMORYMANAGER_HPP
#define __MEMORYMANAGER_HPP

#include <Lib/Bitmap.hpp>
#include <Lib/Singleton.hpp>
#include <Lib/Types.hpp>
#include <MM/Address.hpp>

#define PAGE_SIZE 4096

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

class MemoryManager : public Singleton<MemoryManager> {
public:
    MemoryManager();

    void parseMemoryMap(u32* memoryMap, u32 memoryMapSize);
    PhysicalAddress* getFreePhysicalPage(u32 size = 1);

private:
    Bitmap* physicalMemoryMap;
};

#endif
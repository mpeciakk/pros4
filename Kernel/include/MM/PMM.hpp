#ifndef __PMM_HPP
#define __PMM_HPP

#include <Lib/Types.hpp>
#include <Lib/Singleton.hpp>

class PhysicalMemoryManager : public Singleton<PhysicalMemoryManager> {
public:
    PhysicalMemoryManager();

    void parseMemoryMap(u32* memoryMap, u32 memoryMapSize);
};

#endif
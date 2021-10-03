#ifndef __GDT_HPP
#define __GDT_HPP

#include <Lib/Types.hpp>

#define MAX_DESCRIPTORS 6

class GDT {
public:
    GDT();
    ~GDT() = default;

private:
    struct Descriptor {
        u16 limit;
        u16 baseLow;
        u8 baseMid;
        u8 flags;
        u8 grand;
        u8 baseHigh;
    } __attribute__((packed));

    struct GDTPointer {
        u16 size;
        u32 base;
    } __attribute__((packed));

    Descriptor descriptors[MAX_DESCRIPTORS];

    void setDescriptor(u32 i, u64 base, u64 limit, u8 access, u8 grand);
};

#endif
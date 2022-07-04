#ifndef __ADDRESS_HPP
#define __ADDRESS_HPP

#include <MM/MemoryManager.hpp>

class PhysicalAddress {
public:
    PhysicalAddress(u32 address) : address(address) { }
    PhysicalAddress() : address(0) { }

    u32 get() {
        return address;
    }

    u32* ptr() {
        return (u32*) address;
    }

private:
    u32 address;
};

class VirtualAddress {
public:
    VirtualAddress(u32 address) : address(address) { }
    VirtualAddress() : address(0) { }

    u32 get() {
        return address;
    }

    u32* ptr() {
        return (u32*) address;
    }

private:
    u32 address;
};

class PhysicalPage {
public:
    PhysicalPage(PhysicalAddress address) : address(address) { }

    PhysicalAddress addr() {
        return address;
    }

private:
    PhysicalAddress address;
};

#endif
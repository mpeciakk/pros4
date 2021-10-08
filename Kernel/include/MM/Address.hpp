#ifndef __ADDRESS_HPP
#define __ADDRESS_HPP

class PhysicalAddress {
public:
    PhysicalAddress(u32 address) : address(address) {}
    PhysicalAddress() : address(0) {}

    u32 get() {
        return address;
    }

    u8* ptr() {
        return (u8*) address;
    }

private:
    u32 address;
};

class PhysicalPage {
public:
    PhysicalPage(PhysicalAddress address) : address(address) {}

    PhysicalPage addr() {
        return address;
    }

    void free() {

    }

private:
    PhysicalAddress address;
};

#endif
typedef u32 PhysicalPtr;

class PhysicalAddress {
public:
    PhysicalAddress(PhysicalPtr address) : address(address) {}

    PhysicalPtr get() {
        return address;
    }

    u8* ptr() {
        return (u8*) address;
    }

private:
    PhysicalPtr address;
};
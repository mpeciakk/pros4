#include <Hardware/GDT.hpp>

extern "C" [[noreturn]] void kmain(void* mbi, unsigned int multibootMagic) {
    GDT gdt;

    while (true) {
    }
}
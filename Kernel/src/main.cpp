#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>

extern "C" [[noreturn]] void kmain(void* mbi, unsigned int multibootMagic) {
    GDT gdt;

    InterruptManager idt;
    idt.activate();

    while (true) {
    }
}
#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>
#include <Terminal.hpp>
#include <Multiboot.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, unsigned int multibootMagic) {
    Terminal terminal;
    terminal.clear();

    GDT gdt;

    InterruptManager idt;
    idt.activate();

    terminal.write("Kernel loaded!");

    while (true) {
    }
}
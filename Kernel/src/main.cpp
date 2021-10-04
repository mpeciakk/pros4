#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>
#include <Terminal.hpp>
#include <Multiboot.hpp>
#include <Lib/Log.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, unsigned int multibootMagic) {
    Terminal terminal;
    terminal.clear();

    GDT gdt;

    InterruptManager idt;
    idt.activate();

    klog(0, "Kernel loaded!");
    log(0, "Kernel loaded!");

    while (true) {
    }
}
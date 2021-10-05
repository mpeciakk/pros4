#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>
#include <Lib/Log.hpp>
#include <MM/PMM.hpp>
#include <Multiboot.hpp>
#include <Terminal.hpp>

extern "C" [[noreturn]] void kmain(multiboot_info* mbi, unsigned int multibootMagic) {
    Terminal terminal;
    terminal.clear();

    GDT gdt;

    int stack = 0;
    asm volatile("mov %%esp, %0" : "=r"(stack));
    setTSS(0x10, stack);

    InterruptManager idt;
    idt.activate();

    PhysicalMemoryManager pmm;
    pmm.parseMemoryMap((u32*) mbi->mmap_addr, mbi->mmap_length);

    klog(0, "Kernel loaded!");
    log(0, "Kernel loaded!");

    while (true) {
    }
}
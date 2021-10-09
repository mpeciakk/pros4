#include <Exceptions.hpp>
#include <Hardware/GDT.hpp>
#include <Hardware/IDT.hpp>
#include <Lib/Log.hpp>
#include <Lib/Vector.hpp>
#include <MM/MemoryManager.hpp>
#include <MM/kmalloc.hpp>
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

    BasicMemoryAllocator bma;

    idt.handlers[14] = new PageFaultHandler();

    MemoryManager mm;
    mm.parseMemoryMap((u32*) mbi->mmap_addr, mbi->mmap_length);

    klog(0, "Kernel loaded!");
    log(0, "Kernel loaded!");

    int* a = (int*) 0xC2137420;
    *a = 2137;

    while (true) {
    }
}
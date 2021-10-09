#include <Exceptions.hpp>
#include <Lib/Log.hpp>
#include <Hardware/IDT.hpp>

PageFaultHandler::PageFaultHandler() : InterruptHandler() { }

u32 PageFaultHandler::handle(u32 esp) {
    auto* frame = (TrapFrame*) esp;

    u32 cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    auto error = frame->error;

    klog(2, "Kernel bruh moment: PAGE FAULT");
    klog(2, "Page is %s, thrown when %s in %s mode", error & (1 << 0) ? "present" : "not present", error & (1 << 1) ? "writing" : "reading", error & (1 << 2) ? "user" : "kernel");
    klog(2, "Faulting instruction: 0x%x", frame->eip);
    klog(2, "Faulting address: 0x%x", cr2);

    return esp;
}

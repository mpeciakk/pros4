#include <Exceptions.hpp>
#include <Lib/Log.hpp>
#include <Hardware/IDT.hpp>

PageFaultHandler::PageFaultHandler() : InterruptHandler() { }

u32 PageFaultHandler::handle(u32 esp) {
    auto* frame = (TrapFrame*) esp;

    //    klog(2,
    //         "------------%s------------\n"
    //         "eax: 0x%x, ebx: 0x%x, ecx: 0x%x, edx: 0x%x\n"
    //         "esi: 0x%x, edi: 0x%x, ebp: 0x%x\n"
    //         "error: 0x%x, flags: 0x%x, eip: 0x%x\n"
    //         "cs: 0x%x, esp: 0x%x, ss: 0x%x",
    //         exceptions[exception], frame->eax, frame->ebx, frame->ecx, frame->edx, frame->esi, frame->edi, frame->ebp, frame->error,
    //         frame->eflags, frame->eip, frame->cs, frame->esp, frame->ss);

    u32 cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    auto error = frame->error;

    klog(2, "Kernel bruh moment: PAGE FAULT");
    klog(2, "Page is %s, thrown when %s in %s mode", error & (1 << 0) ? "present" : "not present", error & (1 << 1) ? "writing" : "reading", error & (1 << 2) ? "user" : "kernel");
    klog(2, "Faulting instruction: 0x%x", frame->eip);
    klog(2, "Faulting address: 0x%x", cr2);

    return esp;
}

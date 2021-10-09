#ifndef __EXCEPTIONS_HPP
#define __EXCEPTIONS_HPP

#include <Hardware/IDT.hpp>

class PageFaultHandler : public InterruptHandler {
public:
    PageFaultHandler();

    u32 handle(u32 esp) override;
};

#endif
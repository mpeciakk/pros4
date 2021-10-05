#ifndef __LOG_HPP
#define __LOG_HPP

#include <Lib/String.hpp>
#include <Lib/Types.hpp>
#include <Terminal.hpp>
#include <stdarg.h>

void log(int level, const char* __restrict format, ...);
void klog(int level, const char* __restrict format, ...);

#endif
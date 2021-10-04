#ifndef __LOG_HPP
#define __LOG_HPP

#include <Lib/String.hpp>
#include <Lib/Types.hpp>
#include <Terminal.hpp>
#include <stdarg.h>

void print(void (*printFunction)(char* text, u32 length), const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }

            u32 amount = 1;

            while (format[amount] && format[amount] != '%') {
                amount++;
            }

            printFunction((char*) format, amount);
            format += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            printFunction(&c, sizeof(c));
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            printFunction((char*) str, len);
        } else if (*format == 'd') {
            // TODO
        } else if (*format == 'x') {
            // TODO
        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            printFunction((char*) format, len);
            format += len;
        }
    }

    va_end(parameters);
}

void rawPrint(char* data, u32 length) {
    for (u32 i = 0; i < length; i++) {
        Terminal::instance->putChar(data[i]);
    }
}

void rawKPrint(char* data, u32 length) {
    const auto* bytes = (const u8*) data;
    Port8Bit port(0x3F8);

    for (u32 i = 0; i < length; i++) {
        port.write(bytes[i]);
    }
}

template<typename ...Args>
void log(int level, const char* __restrict format, Args... args) {
    switch (level) {
        case 0:
            print(rawPrint, "[");
            Terminal::instance->setTextColor(VGA_COLOR_GREEN);
            print(rawPrint, " LOG ");
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            print(rawPrint, "] ");
            break;
        case 1:
            print(rawPrint, "[");
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_RED);
            print(rawPrint, " WARN ");
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            print(rawPrint, "] ");
            break;
        case 2:
            print(rawPrint, "[");
            Terminal::instance->setTextColor(VGA_COLOR_RED);
            print(rawPrint, " ERR ");
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            print(rawPrint, "] ");
            break;
        default:
            break;
    }

    print(rawPrint, format, args...);
}

template<typename ...Args>
void klog(int level, const char* __restrict format, Args... args) {
    switch (level) {
        case 0:
            print(rawKPrint, "[");
            print(rawKPrint, "\033[32mLOG\033[39m");
            print(rawKPrint, "] ", 2);
            break;
        case 1:
            print(rawKPrint, "[");
            print(rawPrint, "\033[33mWARN\033[39m");
            print(rawKPrint, "] ");
            break;
        case 2:
            print(rawKPrint, "[", 1);
            print(rawKPrint, "\033[31mERR\033[39m");
            print(rawKPrint, "] ");
            break;
        default:
            break;
    }

    print(rawKPrint, format, args...);
}

#endif
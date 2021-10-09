#include <Lib/Log.hpp>

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

void internalPrintf(void (*printFunction)(char* text, u32 length), const char* __restrict format, va_list parameters) {
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
            format++;
            int n = va_arg(parameters, int);
            int numChars = countDigit(n);

            char str[numChars];
            itoa(n, str, 10);

            printFunction(str, numChars);
        } else if (*format == 'x') {
            format++;
            u32 n = va_arg(parameters, u32);
            int numChars = countDigit(n, 16);

            char str[numChars];
            itoa(n, str, 16);

            printFunction(str, numChars);
        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            printFunction((char*) format, len);
            format += len;
        }
    }
}

void log(int level, const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    switch (level) {
        case 0:
            internalPrintf(rawPrint, "[", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_GREEN);
            internalPrintf(rawPrint, " LOG ", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            internalPrintf(rawPrint, "] ", parameters);
            break;
        case 1:
            internalPrintf(rawPrint, "[", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_RED);
            internalPrintf(rawPrint, " WARN ", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            internalPrintf(rawPrint, "] ", parameters);
            break;
        case 2:
            internalPrintf(rawPrint, "[", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_RED);
            internalPrintf(rawPrint, " ERR ", parameters);
            Terminal::instance->setTextColor(VGA_COLOR_LIGHT_GREY);
            internalPrintf(rawPrint, "] ", parameters);
            break;
        default:
            break;
    }

    internalPrintf(rawPrint, format, parameters);
    internalPrintf(rawPrint, "\n", parameters);
    va_end(parameters);
}

void klog(int level, const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    switch (level) {
        case 0:
            internalPrintf(rawKPrint, "[\033[32mLOG\033[39m] ", parameters);
            break;
        case 1:
            internalPrintf(rawKPrint, "[\033[33mWARN\033[39m] ", parameters);
            break;
        case 2:
            internalPrintf(rawKPrint, "[\033[31mERR\033[39m] ", parameters);
            break;
        case 3:
            internalPrintf(rawKPrint, "[\033[36mDBG\033[39m] ", parameters);
            break;
        default:
            break;
    }

    internalPrintf(rawKPrint, format, parameters);
    internalPrintf(rawKPrint, "\n", parameters);
    va_end(parameters);
}
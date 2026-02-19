#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <debug/printf.h>

#include <sys/debug/print.h>

void printf(const char * format, ...) {
    va_list args;

    bool in_insert = false;

    for (va_start(args, format); *format != '\0'; ++format) {
        if (!in_insert) {
            if (*format == '%') {
                in_insert = true;
            }
            else {
                debug_print_char(*format);
            }
        }
        else {
            switch(*format) {
                case 'i': {
                    int i = va_arg(args, int);

                    debug_print_dec(i);

                    in_insert = false;
                } break;

                case 'c': {
                    char c = va_arg(args, int);

                    debug_print_char(c);

                    in_insert = false;
                } break;

                case 's': {
                    const char * str = va_arg(args, const char *);

                    debug_print(str);

                    in_insert = false;
                } break;

                case '%': {
                    debug_print("%");

                    in_insert = false;
                } break;

                default: {
                    debug_print("??");

                    in_insert = false;
                } break;
            }
        }
    }

        va_end(args);
}

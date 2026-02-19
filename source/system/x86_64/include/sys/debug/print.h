#pragma once

#include <stddef.h>

#include <sys/asm/out.h>
#include <sys/asm/in.h>

static inline void debug_print(const char * msg) {
    for (size_t i = 0; msg[i] != '\0'; i++) {
        while ((inb(0x3F8 + 5) & 0x20) == 0) { }

        outb(0x3F8, msg[i]);
    }
}

static inline void debug_print_char(char c) {
    outb(0x3F8, c);
}

static inline void debug_print_dec(uint64_t num) {
    if (num == 0) {
        debug_print("0");
        return;
    }

    char buffer[30];
    buffer[29] = '\0';

    uint64_t position = 28;
    while (num != 0) {
        if (num % 10 > 9) buffer[position] = (char) ('A' + num % 10 - 10);
        else buffer[position] = (char) ('0' + num % 10);

        position--;
        num /= 10;
    }

    debug_print(buffer + position + 1);
}

static inline void debug_print_hex(uint64_t num) {
    if (num == 0) {
        debug_print("0");
        return;
    }

    char buffer[20];
    buffer[19] = '\0';

    uint64_t position = 18;
    while (num != 0) {
        if (num % 16 > 9) buffer[position] = (char) ('A' + num % 16 - 10);
        else buffer[position] = (char) ('0' + num % 16);

        position--;
        num /= 16;
    }

    debug_print(buffer + position + 1);
}

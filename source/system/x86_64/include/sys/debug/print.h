#pragma once

#include <stddef.h>

#include <sys/asm/out.h>

static inline void debug_print(const char * msg) {
    for (size_t i = 0; msg[i] != '\0'; i++) {
        outb(0x3F8, msg[i]);
    }
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

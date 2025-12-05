#pragma once

#include <stddef.h>

#include <device/device.h>

#include <util/string/strlen.h>

static inline void vga_print(const char * msg) {
    extern device_t * vga_term_device;

    if (vga_term_device != NULL) {
        uint64_t position = 0;
        uint64_t length = strlen(msg);

        while (position < length) {
            position += vga_term_device->char_ops.write(vga_term_device, msg + position, length - position);
        }
    }
}

static inline void vga_print_hex(uint64_t num) {
    if (num == 0) {
        vga_print("0");
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

    vga_print(buffer + position + 1);
}

#pragma once

#include <stdint.h>

static inline void flash(uint8_t color) {
    #define VIDEO_MEMORY ((uint8_t *) 0xA0000)

    for (uint32_t i = 0; i < 320 * 200; i++) VIDEO_MEMORY[i] = color;

    #undef VIDEO_MEMORY

    for (uint64_t i = 0; i < 1000000000; i++) asm volatile ("nop");
}
#pragma once

#include <stdint.h>

static inline void ltr(uint16_t tss_offset) {
    asm volatile ("ltr %0" : : "m" (tss_offset));
}
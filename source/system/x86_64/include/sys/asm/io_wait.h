#pragma once

#include <sys/asm/out.h>

static inline void io_wait(void) {
    outb(0x80, 0);
}
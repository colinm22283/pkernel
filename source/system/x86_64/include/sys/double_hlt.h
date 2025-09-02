#pragma once

#include <sys/asm/hlt.h>

static inline void double_hlt() {
    hlt();
    hlt();
}
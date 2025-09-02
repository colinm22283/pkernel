#pragma once

#include <sys/asm/hlt.h>

static inline void wait_for_interrupt() {
    hlt();
}
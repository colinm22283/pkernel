#pragma once

#include <sys/asm/hlt.h>
#include <sys/asm/sti.h>

static inline void wait_for_interrupt() {
    sti();
    hlt();
}
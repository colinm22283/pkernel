#pragma once

static inline void nop() {
    asm volatile ("nop");
}
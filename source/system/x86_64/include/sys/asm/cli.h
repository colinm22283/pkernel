#pragma once

static inline void cli() {
    asm volatile ("cli");
}
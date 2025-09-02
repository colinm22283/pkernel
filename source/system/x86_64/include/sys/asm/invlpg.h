#pragma once

static inline void invlpg(void * address) {
    asm volatile ("invlpg (%0)" : : "b" (address) : "memory");
}
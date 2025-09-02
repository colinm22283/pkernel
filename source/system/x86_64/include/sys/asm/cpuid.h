#pragma once

#include <stdint.h>

static inline void cpuid(uint32_t code, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx) {
    asm volatile(
        "cpuid" :
        "=a" (code), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) :
        "0" (*eax), "2" (*ecx)
    );
}
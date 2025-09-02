#pragma once

#include <stdint.h>

#include <sys/msr/msr_numbers.h>

static inline uint64_t read_msr(msr_number_t msr) {
    uint32_t upper, lower;
    asm volatile ("rdmsr" : "=a" (lower), "=d" (upper) : "c" (msr));
    return (((uint64_t) upper) << 32) | lower;
}
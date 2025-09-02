#pragma once

#include <stdint.h>

#include <sys/msr/msr_numbers.h>

static inline void write_msr(msr_number_t msr, uint64_t _value) {
    uint32_t * const value = (uint32_t *) &_value;

    asm volatile ("wrmsr" : : "a" (value[0]), "d" (value[1]), "c" (msr));
}
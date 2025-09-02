#pragma once

#include <stdint.h>
#include <sys/msr/write_msr.h>
#include <sys/msr/read_msr.h>

static inline void set_msr(msr_number_t msr, uint64_t bits) {
    write_msr(msr, read_msr(msr) & ~bits);
}
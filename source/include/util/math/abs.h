#pragma once

#include <stdint.h>

static inline uint32_t math_abs(int32_t x) {
    return __builtin_abs(x);
}
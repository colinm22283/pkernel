#pragma once

#include <defs.h>

__ALWAYS_INLINE static inline void load_stack_pointer(void * stack_pointer) {
    asm volatile ("movabs %0, %%rsp" : : "i" (stack_pointer) : "memory");
    asm volatile ("movabs %0, %%rbp" : : "i" (stack_pointer) : "memory");
}
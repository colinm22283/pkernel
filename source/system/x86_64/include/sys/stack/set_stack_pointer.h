#pragma once

#include <defs.h>

__ALWAYS_INLINE static inline void set_stack_pointer(void * stack_pointer) {
    asm volatile ("mov %0, %%rsp" : : "r" ((intptr_t) stack_pointer) : "memory");
    asm volatile ("mov %0, %%rbp" : : "r" ((intptr_t) stack_pointer) : "memory");
}
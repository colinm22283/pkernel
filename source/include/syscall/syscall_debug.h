#pragma once

#include <sys/debug/print.h>

#define SYSCALL_DEBUG

static inline void syscall_debug_print(const char * str) {
#ifdef SYSCALL_DEBUG
    debug_print(str);
#endif
}

static inline void syscall_debug_print_hex(uint64_t num) {
#ifdef SYSCALL_DEBUG
    debug_print_hex(num);
#endif
}

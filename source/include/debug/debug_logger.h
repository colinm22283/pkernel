#pragma once

#include <debug/printf.h>

#include <defs.h>

#ifdef DEBUG_LOGGER_ENABLED

#define DEFINE_KERNEL_PRINTF(str) \
    static inline void kprintf(const char * format, ...) { \
        printf("[KERNEL %s] ", (str)); \
        va_list args; \
        va_start(args, format); \
        vprintf(format, args); \
        va_end(args); \
        printf("\n"); \
    }


#else

#define DEFINE_KERNEL_PRINTF(str) static inline void kprintf(__MAYBE_UNUSED const char * format, ...) { }

#endif

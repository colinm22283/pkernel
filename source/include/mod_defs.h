#pragma once

#include <stddef.h>

#include <debug/printf.h>

#include <defs.h>

#include <sys/debug/print.h>

#include <config.h>

extern const char module_name[];

#define __MOD_EXPORT __SECTION(".module_export")

#define MODULE_NAME(name) const char module_name[] = (name)

#define MODULE_DEPS_NONE() const char * module_deps[0]; size_t module_dep_count = 0
#define MODULE_DEPS(...) const char * module_deps[] = { __VA_ARGS__ }; const size_t module_dep_count = sizeof(module_deps) / sizeof(const char *)

#ifdef DEBUG

#define MODULE_DEBUG(...) do { debug_print("[MODULE "); debug_print(module_name); debug_print("] "); __VA_ARGS__; debug_print("\n"); } while (0)

#define MODULE_PRINT(msg) debug_print(msg)
#define MODULE_PRINT_HEX(num) debug_print_hex(num)

static inline void kprintf(const char * format, ...) {
    printf("[MODULE %s] ", module_name);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

#else

#define MODULE_DEBUG(...)

#define MODULE_PRINT(msg)
#define MODULE_PRINT_HEX(num)

static inline void kprintf(const char * format, ...) { }

#endif

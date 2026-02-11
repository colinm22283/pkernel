#pragma once

#include <stddef.h>

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

#else

#define MODULE_DEBUG(...)

#define MODULE_PRINT(msg)
#define MODULE_PRINT_HEX(num)

#endif

#pragma once

#include <stddef.h>

#include <defs.h>

#define __MOD_EXPORT __SECTION(".module_export")

#define MODULE_NAME(name) const char module_name[] = (name)

#define MODULE_DEPS_NONE() const char * module_deps[0]; size_t module_dep_count = 0
#define MODULE_DEPS(...) const char * module_deps[] = { __VA_ARGS__ }; const size_t module_dep_count = sizeof(module_deps) / sizeof(const char *)
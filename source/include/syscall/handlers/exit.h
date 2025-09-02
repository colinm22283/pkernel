#pragma once

#include <stdint.h>

#include <defs.h>

__NORETURN void syscall_exit(uint64_t exit_code);
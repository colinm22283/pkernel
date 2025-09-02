#pragma once

#include <stdint.h>

extern char _kernel_size;
#define KERNEL_SIZE ((uint64_t) &_kernel_size)

extern char _kernel_start;
#define KERNEL_START ((void *) &_kernel_start)

extern char _kernel_end;
#define KERNEL_END ((void *) &_kernel_end)
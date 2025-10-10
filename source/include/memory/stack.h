#pragma once

#include <stdint.h>

extern char _stack_base;
#define STACK_BASE ((void *) &_stack_base)

extern char _stack_top;
#define STACK_TOP ((void *) &_stack_top)

extern char _stack_size;
#define STACK_SIZE ((uint64_t) &_stack_size)

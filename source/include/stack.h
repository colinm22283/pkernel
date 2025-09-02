#pragma once

#include <stdint.h>

#include <defs.h>

extern intptr_t _stack_top;
extern intptr_t _stack_base;
#define stack_top (&_stack_top)
#define stack_base (&_stack_base)
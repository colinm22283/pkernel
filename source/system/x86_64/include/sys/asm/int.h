#pragma once

#include <stdint.h>

#define asm_int(code) do { asm volatile ("int %0" : : "i" (code)); } while (0)
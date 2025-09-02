#pragma once

#include <stdint.h>

static inline void sidt(void * idt_ptr_address) {
    asm volatile ("sidt %w0" : : "Nd" (idt_ptr_address) : "memory");
}
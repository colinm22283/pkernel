#pragma once

#include <sys/idt/idt_ptr.h>

static inline void lidt(idt_ptr_t * const _idt_ptr) {
    asm volatile ("lidt %0" : : "m" (*_idt_ptr));
}
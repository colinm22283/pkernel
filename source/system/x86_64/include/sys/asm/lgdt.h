#pragma once

#include <sys/gdt/gdt64.h>

static inline void lgdt(gdt64_ptr_t * const gdt_ptr) {
    asm volatile ("lgdt %0" : : "m" (*gdt_ptr));
}
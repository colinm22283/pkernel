#pragma once

#include <stdint.h>
#include <stddef.h>

#include <sys/asm/hlt.h>

#include <debug/printf.h>

static inline void load_page_table(uint64_t page_table) {
    asm volatile ("mov %0, %%cr3" : : "r" (page_table) : "memory");
}

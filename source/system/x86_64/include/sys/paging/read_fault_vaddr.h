#pragma once

#include <stdint.h>

static inline void * read_fault_vaddr() {
    uint64_t page_table;
    asm volatile ("mov %%cr2, %0" : "=r" (page_table));
    return (void *) page_table;
}
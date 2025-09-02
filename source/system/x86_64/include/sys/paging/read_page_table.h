#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/paging/pml4t.h>

static inline uint64_t read_page_table() {
    uint64_t page_table;
    asm volatile ("mov %%cr3, %0" : "=r" (page_table));
    return page_table;
}
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <paging/region.h>
#include <paging/virtual_reservations.h>

#include <util/math/min.h>

#include <error_number.h>

extern uint64_t bitmap_paddr;
extern uint64_t bitmap_available_pages;
extern uint64_t bitmap_size;

static inline bool paging_bitmap_get_index(uint64_t index) {
    return (PAGING_BITMAP_VADDR[(index) / 64] >> (index % 64)) & 0b1;
}
static inline void paging_bitmap_set_index(uint64_t index) {
    PAGING_BITMAP_VADDR[index / 64] |= 0b1 << (index % 64);
}
static inline void paging_bitmap_clear_index(uint64_t index) {
    PAGING_BITMAP_VADDR[index / 64] &= ~(0b1 << (index % 64));
}

void paging_bitmap_init(void);

uint64_t bitmap_reserve(void);
uint64_t bitmap_reserve_contiguous(uint64_t size_pages);
error_number_t bitmap_free(uint64_t address, uint64_t size_pages);

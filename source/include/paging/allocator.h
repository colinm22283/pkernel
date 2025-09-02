#pragma once

#include <paging/mapper.h>
#include <paging/_virtual_allocator.h>

#include <util/math/div_up.h>

typedef struct {
    void * vaddr;
    uint64_t size_pages;

    uint64_t mapping_count;
    paging_mapping_t * mappings;

    uint64_t bitmap_level;
    uint64_t paddr_count;
    uint64_t * paddrs;
} paging_allocation_t;

bool paging_alloc_pages(paging_allocation_t * allocation, uint64_t size_pages);
static inline bool paging_alloc(paging_allocation_t * allocation, uint64_t size_bytes) {
    return paging_alloc_pages(allocation, DIV_UP(size_bytes, 0x1000));
}

bool paging_free(paging_allocation_t * allocation);
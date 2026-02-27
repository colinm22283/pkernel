#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <paging/table_allocator.h>
#include <paging/tables.h>

#include <sys/paging/pml4t.h>

typedef struct {
    uint64_t size_pages;
    uint64_t allocation_count;
    paging_table_allocation_t * allocations;
    void * vaddr;
} paging_mapping_t;

bool paging_map_ex(pml4t64_t * pml4t, paging_mapping_t * mapping, uint64_t paddr, void * vaddr, uint64_t size_pages, bool read_write, bool execute_disable, bool user_super);
static inline bool paging_map(pml4t64_t * pml4t, paging_mapping_t * mapping, uint64_t paddr, void * vaddr, uint64_t size_pages) {
    return paging_map_ex(pml4t, mapping, paddr, vaddr, size_pages, true, false, false);
}

void paging_unmap(pml4t64_t * pml4t, paging_mapping_t * mapping);
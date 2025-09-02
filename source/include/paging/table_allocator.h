#pragma once

#include <stdint.h>

#include <sys/paging/pt.h>

typedef struct {
    pt64_t * pt;
    uint64_t paddr;
    void * vaddr;
} paging_table_allocation_t;

void paging_talloc_init(void);

bool paging_talloc_alloc(paging_table_allocation_t * alloc);
void paging_talloc_free(paging_table_allocation_t * allocation);

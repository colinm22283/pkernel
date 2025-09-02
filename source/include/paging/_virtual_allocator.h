#pragma once

#include <stdint.h>

void paging_valloc_init(void);
void * paging_valloc_alloc(uint64_t size_pages);
void paging_valloc_free(void * vaddr, uint64_t size_pages);

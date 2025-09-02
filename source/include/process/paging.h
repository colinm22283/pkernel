#pragma once

#include <paging/allocator.h>
#include <paging/mapper.h>

#include <sys/paging/pml4t.h>

typedef struct {
    paging_allocation_t kernel_allocation;

    void * vaddr;

    uint64_t user_mapping_count;
    paging_mapping_t * user_mappings;
} process_paging_allocation_t;

typedef struct {
    paging_allocation_t pml4t_alloc;
} process_paging_t;

bool process_paging_init(process_paging_t * paging_data);
void process_paging_free(process_paging_t * paging_data);

bool process_paging_alloc_ex(process_paging_t * paging_data, process_paging_allocation_t * allocation, uint64_t size_bytes, bool read_write, bool execute_disable);
bool process_paging_alloc_static_ex(process_paging_t * paging_data, process_paging_allocation_t * allocation, void * user_address, uint64_t size_bytes, bool read_write, bool execute_disable);

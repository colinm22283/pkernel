#pragma once

#include <entry.h>

#include <memory/kernel.h>
#include <memory/primary_region.h>

#include <paging/tables.h>

static inline uint64_t paging_kernel_virtual_to_physical(void * virtual_address) {
    return (uint64_t) ((intptr_t) virtual_address + primary_region_start - (intptr_t) KERNEL_START);
}

static inline void * paging_kernel_physical_to_virtual(uint64_t physical_address) {
    return (void *) (physical_address + (intptr_t) KERNEL_START - primary_region_start);
}
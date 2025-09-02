#pragma once

#include <stdint.h>

#include <defs.h>

#include <sys/memory_map/memory_map.h>

__SECTION(".memory_map")           extern volatile memory_map_entry_t memory_map[(0x200 - 4) / sizeof(memory_map_entry_t)];
__SECTION(".memory_map_size")      extern volatile const uint32_t     memory_map_size;
__SECTION(".kernel_region_base")   extern volatile uint64_t           kernel_region_base;
__SECTION(".kernel_region_length") extern volatile uint64_t           kernel_region_length;

typedef struct {
    uint64_t base, length;
} primary_memory_region_t;

primary_memory_region_t get_primary_memory_region();

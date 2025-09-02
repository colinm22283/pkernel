#pragma once

#include <stdint.h>

enum {
    MEMORY_MAP_TYPE_USABLE = 1,
    MEMORY_MAP_TYPE_RESERVED = 2,
    MEMORY_MAP_TYPE_ACPI_RECLAIMABLE = 3,
    MEMORY_MAP_TYPE_ACPI_NVS = 4,
    MEMORY_MAP_TYPE_BAD = 5,
};

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} memory_map_entry_t;
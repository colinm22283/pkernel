#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint64_t text_size;
    uint64_t data_size;
    uint64_t rodata_size;
    uint64_t bss_size;
} application_start_table_t;


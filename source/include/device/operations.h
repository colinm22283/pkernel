#pragma once

#include <stdbool.h>

#include <paging/manager.h>

struct device_s;

typedef struct {
    uint64_t (* write)(struct device_s * device, const char * buffer, uint64_t size);
    uint64_t (* read)(struct device_s * device, char * buffer, uint64_t size);

    void * (* map)(struct device_s * device, pman_context_t * context, pman_protection_flags_t prot, void * map_addr, uint64_t size, uint64_t offset);
    int (* unmap)(struct device_s * device, pman_context_t * context, void * map_addr);
} device_char_operations_t;

typedef struct {
    uint64_t (* write)(struct device_s * device, const char * buffer, uint64_t block_size, uint64_t block_offset);
    uint64_t (* read)(struct device_s * device, char * buffer, uint64_t block_size, uint64_t block_offset);
} device_block_operations_t;
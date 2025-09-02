#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <error_number.h>

typedef enum {
    PALLOC_ALLOC,
    PALLOC_CONTIGUOUS_ALLOC,
    PALLOC_CONTIGUOUS_MAP,
} palloc_type_t;

typedef struct {
    palloc_type_t type;

    uint64_t size_pages;

    uint64_t paddr_count;
    uint64_t * paddrs;
} palloc_t;

error_number_t palloc_alloc(palloc_t * palloc, uint64_t size);
error_number_t palloc_alloc_contiguous(palloc_t * palloc, uint64_t size, uint64_t paddr);
error_number_t palloc_free(palloc_t * palloc);
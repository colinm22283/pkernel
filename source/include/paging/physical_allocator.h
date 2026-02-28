#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <errno.h>

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

int palloc_alloc(palloc_t * palloc, uint64_t size);
int palloc_alloc_contiguous(palloc_t * palloc, uint64_t size, uint64_t paddr);
int palloc_free(palloc_t * palloc);
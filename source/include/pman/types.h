#pragma once

#include <stdint.h>

#include <paging/mapper.h>
#include <paging/virtual_allocator.h>
#include <paging/bitmap.h>
#include <paging/physical_allocator.h>

#include <sys/paging/pml4t.h>

#include <defs.h>

struct pman_context_s;

typedef enum {
    MAPPING_BORROW,
    MAPPING_SHARED,
} pman_mapping_type_t;

typedef struct {
    size_t references;

    size_t file_offset;
    file_t * file;
    bool dirty;

    palloc_t * palloc;
    paging_mapping_t * mapping;
} pman_source_t;

typedef struct {
    pman_source_t * source;

    paging_mapping_t mapping;
} pman_mapping_t;

typedef struct pman_context_s {
    paging_table_allocation_t tlt_alloc;
    pml4t64_t tlt;

    valloc_t valloc;

    pman_mapping_t head, tail;
} pman_context_t;


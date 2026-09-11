#pragma once

#include <stdint.h>
#include <stddef.h>

#include <paging/mapper.h>
#include <paging/virtual_allocator.h>
#include <paging/bitmap.h>
#include <paging/physical_allocator.h>

#include <sys/paging/tlt.h>
#include <sys/paging/paddr.h>

#include <defs.h>

struct pman_context_s;
struct fs_file_s;

#define PMAN_BORROW   (1UL << 0)
#define PMAN_SHARED   (1UL << 1)
#define PMAN_VRESERVE (1UL << 2)
typedef int pman_mapping_flags_t;

#define PMAN_EXECUTE (1UL << 0)
#define PMAN_WRITE   (1UL << 1)
typedef int pman_protection_flags_t;

typedef struct {
    size_t references;

    struct fs_file_s * file;
    size_t file_offset;

    palloc_t palloc;
    paging_mapping_t mapping;

    bool initialized;
    bool dirty;
    bool evicted;
} pman_source_t;

typedef struct {
    size_t references;

    void * vaddr;
} pman_virtual_range_t;

typedef struct pman_mapping_s {
    struct pman_context_s * context;

    pman_mapping_flags_t flags;
    pman_protection_flags_t prot;
    pman_source_t * source;

    void * vaddr;
    pman_virtual_range_t * vrange;

    paging_mapping_t mapping;

    bool active;

    struct pman_mapping_s * next;
    struct pman_mapping_s * prev;
} pman_mapping_t;

typedef struct pman_context_s {
    paging_table_allocation_t tlt_alloc;
    paddr_t tlt_paddr;
    pml4t64_t * tlt;

    valloc_t valloc;

    pman_mapping_t head, tail;
} pman_context_t;

typedef struct {
    size_t size;
    pman_mapping_t ** mappings;


} pman_range_t;


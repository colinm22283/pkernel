#pragma once

#include <paging/mapper.h>
#include <paging/virtual_allocator.h>
#include <paging/bitmap.h>
#include <paging/physical_allocator.h>

#include <error_number.h>

#include <sys/paging/pml4t.h>
#include <sys/exceptions/page_fault_error_code.h>
#include <sys/isr/isr.h>

#include <defs.h>

struct pman_context_s;
struct process_s;

typedef error_number_t (* pman_mapping_write_handler_t)(uint64_t address_offset);
typedef error_number_t (* pman_mapping_read_handler_t)(uint64_t address_offset);

typedef enum {
    PMAN_PROT_WRITE   = 0b00001,
    PMAN_PROT_EXECUTE = 0b00010,
} pman_protection_flags_t;

typedef enum {
    PMAN_MAPPING_ALLOC,
    PMAN_MAPPING_MAP,
    PMAN_MAPPING_BORROWED,
    PMAN_MAPPING_SHARED,
    PMAN_MAPPING_HANDLER,
} pman_mapping_type_t;

typedef struct pman_mapping_s {
    struct pman_context_s * context;

    pman_mapping_type_t type;
    pman_protection_flags_t protection;

    void * vaddr;
    uint64_t size_pages;

    union {
        struct {
            uint64_t references;

            palloc_t palloc;

            uint64_t mapping_count;
            paging_mapping_t * mappings;
        } alloc;

        struct {
            uint64_t references;

            palloc_t palloc;

            paging_mapping_t mapping;
        } map;

        struct {
            struct pman_mapping_s * lender;

            uint64_t mapping_count;
            paging_mapping_t * mappings;
        } borrowed;

        struct {
            struct pman_mapping_s * lender;

            uint64_t mapping_count;
            paging_mapping_t * mappings;
        } shared;

        struct {
            pman_mapping_write_handler_t * write;
            pman_mapping_read_handler_t * read;
        } handler;
    };

    struct pman_mapping_s * next;
    struct pman_mapping_s * prev;
} pman_mapping_t;

typedef struct pman_context_s {
    paging_table_allocation_t top_level_table_allocation;
    pml4t64_t * top_level_table;
    uint64_t top_level_table_paddr;

    valloc_t valloc;

    pman_mapping_t head, tail;
} pman_context_t;

static inline pman_mapping_t * get_root_mapping(pman_mapping_t * mapping) {
    switch (mapping->type) {
        case PMAN_MAPPING_BORROWED: return get_root_mapping(mapping->borrowed.lender);
        case PMAN_MAPPING_SHARED: return get_root_mapping(mapping->shared.lender);

        default: return mapping;
    }
}

static inline void pman_add_reference(pman_mapping_t * mapping) {
    switch (mapping->type) {
        case PMAN_MAPPING_ALLOC: mapping->alloc.references++; break;
        case PMAN_MAPPING_MAP: mapping->map.references++; break;

        default: __UNREACHABLE();
    }
}

void pman_init(void);

pman_context_t * pman_new_context(void);
error_number_t pman_free_context(pman_context_t * context);

void pman_context_load_table(pman_context_t * context);

pman_mapping_t * pman_context_add_alloc(pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t size);
pman_mapping_t * pman_context_add_map(pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t paddr, uint64_t size);
pman_mapping_t * pman_context_add_borrowed(pman_context_t * context, pman_protection_flags_t prot, pman_mapping_t * lender, void * vaddr);
pman_mapping_t * pman_context_add_shared(pman_context_t * context, pman_protection_flags_t prot, pman_mapping_t * lender, void * vaddr);
pman_mapping_t * pman_context_add_handler(pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t paddr);

error_number_t pman_context_unmap(pman_mapping_t * mapping);

pman_mapping_t * pman_context_resize(pman_mapping_t * mapping, uint64_t size);

pman_mapping_t * pman_context_get_vaddr(pman_context_t * context, void * vaddr);

pman_mapping_t * pman_context_prepare_write(struct process_s * process, pman_mapping_t * mapping);

static inline pman_context_t * pman_kernel_context(void) {
    extern pman_context_t kernel_context;

    return &kernel_context;
}

#pragma once

#include <pman/types.h>

void pman_init(void);

static inline pman_context_t * pman_kernel_context(void) {
    extern pman_context_t kernel_context;

    return &kernel_context;
}

pman_context_t * pman_new_context(void);
int pman_free_context(pman_context_t * context);

void pman_context_load_table(pman_context_t * context);
void pman_fork_context(pman_context_t * dst, pman_context_t * src);

pman_range_t * pman_add_anon_map(
    pman_context_t * context,
    void * vaddr,
    size_t size_bytes,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
);
pman_range_t * pman_copy_range(
    pman_context_t * context,
    pman_range_t * range,
    void * vaddr,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
);
pman_range_t * pman_map_region(
    pman_context_t * context,
    void * vaddr,
    size_t size_bytes,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
);

int pman_unmap(pman_context_t * context, void * vaddr, size_t size_bytes);


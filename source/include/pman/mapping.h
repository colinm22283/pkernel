#pragma once

#include <pman/types.h>

#include <util/heap/heap.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    void * vaddr,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
);

void pman_mapping_free(pman_mapping_t * mapping);

int pman_mapping_prepare_write(pman_mapping_t * mapping);
void pman_mapping_map_in(pman_mapping_t * mapping);


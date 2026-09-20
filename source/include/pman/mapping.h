#pragma once

#include <pman/types.h>

#include <util/heap/heap.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
);

void pman_mapping_free(pman_mapping_t * mapping);


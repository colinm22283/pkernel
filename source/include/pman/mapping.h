#pragma once

#include <pman/types.h>

#include <util/heap/heap.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    pman_virtual_range_t * vrange,
    pman_mapping_flags_t flags
);

void pman_mapping_free(pman_mapping_t * mapping);


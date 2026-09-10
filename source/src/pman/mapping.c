#include <pman/mapping.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    pman_virtual_range_t * vrange,
    pman_mapping_flags_t flags
) {
    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;

    mapping->flags = flags;
    mapping->source = source;

    mapping->vrange = vrange;
    vrange->references++;

    mapping->active = false;

    return mapping;
}

void pman_mapping_free(pman_mapping_t * mapping) {
    if (mapping->active) {
        paging_unmap(mapping->context->tlt, &mapping->mapping);
    }

    mapping->prev->next = mapping->next;
    mapping->next->prev = mapping->prev;
    heap_free(mapping);
}


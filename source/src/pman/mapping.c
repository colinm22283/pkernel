#include <pman/mapping.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
) {
    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;

    mapping->flags = flags;
    mapping->source = source;

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


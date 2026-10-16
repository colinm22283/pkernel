#include <pman/pman.h>
#include <pman/mapping.h>

pman_mapping_t * pman_mapping_init(
    pman_context_t * context,
    pman_source_t * source,
    void * vaddr,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
) {
    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;

    mapping->flags = flags;
    mapping->source = source;

    if (context == pman_kernel_context()) {
        mapping->active = true;

        // TODO: temporary, wont work for mapped files
        paging_map(context->tlt, &mapping->mapping, source->palloc.paddrs[0], vaddr
    }
    else {
        mapping->active = false;
        
        // TODO
    }

    mapping->next = context->head.next;
    mapping->prev = &context->head;
    context->head.next->prev = mapping;
    context->head.next = mapping;

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


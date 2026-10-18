#include <pman/pman.h>
#include <pman/mapping.h>
#include <pman/logger.h>
#include <pman/util.h>

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

    mapping->vaddr = vaddr;

    if (flags | PMAN_ANCHOR) {
        mapping->active = true;

        // TODO: temporary, wont work for mapped files
        paging_map_ex(
            context->tlt,
            &mapping->mapping,
            source->palloc.paddrs[0],
            vaddr,
            1,
            pman_prot_flags_read_write(prot),
            pman_prot_flags_execute_disable(prot),
            true // TODO
        );
    }
    else {
        mapping->active = false;
        
        paging_map_ex(
            context->tlt,
            &mapping->mapping,
            source->palloc.paddrs[0],
            vaddr,
            1,
            pman_prot_flags_read_write(prot),
            pman_prot_flags_execute_disable(prot),
            true // TODO
        );
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


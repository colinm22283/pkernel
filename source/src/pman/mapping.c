#include <pman/pman.h>
#include <pman/mapping.h>
#include <pman/source.h>
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

    mapping->flags  = flags;
    mapping->prot   = prot;

    pman_source_add_ref(source);
    mapping->source = source;

    mapping->vaddr = vaddr;

    mapping->active = false;

    /* if (flags & PMAN_ANCHOR) { */
        /* mapping->active = true; */

        /* // TODO: temporary, wont work for mapped files */
        /* paging_map_ex( */
            /* context->tlt, */
            /* &mapping->mapping, */
            /* source->palloc.paddrs[0], */
            /* vaddr, */
            /* 1, */
            /* pman_prot_flags_read_write(prot), */
            /* pman_prot_flags_execute_disable(prot), */
            /* true // TODO */
        /* ); */
    /* } */
    /* else { */
    /* } */

    mapping->next = context->head.next;
    mapping->prev = &context->head;
    context->head.next->prev = mapping;
    context->head.next = mapping;

    if (flags & PMAN_ANCHOR) pman_mapping_map_in(mapping);

    return mapping;
}

void pman_mapping_free(pman_mapping_t * mapping) {
    kprintf("pman_mapping_free(): mapping->vaddr = %p", mapping->vaddr);
    if (mapping->active) {
        paging_unmap(mapping->context->tlt, &mapping->mapping);
    }

    valloc_release(&mapping->context->valloc, mapping->vaddr);

    pman_source_sub_ref(mapping->source);

    mapping->prev->next = mapping->next;
    mapping->next->prev = mapping->prev;
    heap_free(mapping);
}

int pman_mapping_prepare_write(pman_mapping_t * mapping) {
    kprintf("pman_mapping_prepare_write()");

    if (!(mapping->prot & PMAN_WRITE)) return -EFAULT;

    pman_source_t * new_source = pman_source_prepare_write(mapping->source);
    
    if (new_source == NULL) return -EFAULT;

    mapping->source = new_source;

    return 0;
}

void pman_mapping_map_in(pman_mapping_t * mapping) {
    if (mapping->active) return;

    kprintf("test");

    pman_source_page_in(mapping->source);

    if (mapping->prot & PMAN_WRITE) pman_mapping_prepare_write(mapping);

    kprintf("pman_mapping_map_in() vaddr = %p, paddr = %p, tlt_paddr = %p", mapping->vaddr, mapping->source->palloc.paddrs[0], mapping->context->tlt_paddr);

    kprintf("Setting ex disable to %i", pman_prot_flags_execute_disable(mapping->prot));

    paging_map_ex(
        mapping->context->tlt,
        &mapping->mapping,
        mapping->source->palloc.paddrs[0],
        mapping->vaddr,
        1,
        pman_prot_flags_read_write(mapping->prot),
        pman_prot_flags_execute_disable(mapping->prot),
        true // TODO
    );

    mapping->active = true;
}


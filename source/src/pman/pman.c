#include <pman/pman.h>

#include <interrupt/interrupt_registry.h>

#include <util/memory/memset.h>

#include <sys/panic.h>
#include <sys/paging/map_kernel.h>

#ifdef PMAN_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("paging manager");

pman_context_t kernel_context;

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * isr, void * _error_code);

void pman_init(void) {
    if (!paging_talloc_alloc(&kernel_context.tlt_alloc)) panic0("Unable to allocate pman kern table");

    kernel_context.tlt = kernel_context.tlt_alloc.vaddr;
    kernel_context.tlt_paddr = kernel_context.tlt_alloc.paddr;

    memset(kernel_context.tlt, 0, sizeof(pml4t64_t));

    valloc_init(&kernel_context.valloc);

    sys_paging_map_kernel_regions(&kernel_context);

    kernel_context.head.next = &kernel_context.tail;
    kernel_context.head.prev = NULL;
    kernel_context.tail.next = NULL;
    kernel_context.tail.prev = &kernel_context.head;

    interrupt_registry_register(IC_PAGE_FAULT, pman_page_fault_handler);

    pman_context_load_table(&kernel_context);
}

pman_context_t * pman_new_context(void) {
    return NULL;
}

int pman_free_context(pman_context_t * context) {
    return 0;
}

void pman_context_load_table(pman_context_t * context) {
}

void pman_fork_context(pman_context_t * dst, pman_context_t * src) {
}

pman_range_t * pman_add_anon_map(
    pman_context_t * context,
    void * vaddr,
    size_t size_bytes,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
) {
    size_t size_pages = DIV_UP(size_bytes, PAGE_SIZE);

    pman_source_t ** sources = heap_alloc_debug(size_pages * sizeof(pman_source_t *), "pman_add_anon_map sources");
    pman_mapping_t ** mappings = heap_alloc_debug(size_pages * sizeof(pman_mapping_t *), "pman_add_anon_map mappings");

    for (size_t i = 0; i < size_pages; i++) {
        sources[i] = pman_source_init_anon();
        mappings[i] = pman_mapping_init(context, sources[i], flags, prot);
    }

    pman_range_t * range = heap_alloc_debug(sizeof(pman_range_t), "pman_add_anon_map range");
    range->vaddr = vaddr;
    range->complete = true;
    range->size = size_pages;
    range->mappings = mappings;

    heap_free(sources);

    return range;
}

pman_range_t * pman_copy_range(
    pman_context_t * context,
    pman_range_t * range,
    void * vaddr,
    pman_mapping_flags_t flags,
    pman_protection_flags_t prot
) {
    return NULL;
}

pman_range_t * pman_get_range(
    pman_context_t * context,
    void * vaddr,
    size_t size_bytes
) {
    return NULL;
}

int pman_unmap(pman_context_t * context, void * vaddr, size_t size_bytes) {
    return 0;
}

void pman_range_free(pman_range_t * range) {
}

void pman_range_unmap(pman_range_t * range) {
}

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * isr, void * _error_code) {
    kprintf("FJSDAFKSDAFJSDAKFSDKAJFSADF");
}


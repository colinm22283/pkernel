#include <process/process.h>

#include <util/heap/heap.h>

process_t * process_create(void) {
    process_t * process = heap_alloc(sizeof(process_t));

    process->paging_context = pman_new_context();

    return process;
}

process_t * process_create_fork(process_t * parent) {
    process_t * process = process_create();



    return process;
}

void * process_create_segment(process_t * process, void * vaddr, size_t size, pman_protection_flags_t prot) {
    pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, size);
    pman_mapping_t * user_mapping = pman_context_add_shared(process->paging_context, prot, kernel_mapping, vaddr);
    pman_context_unmap(kernel_mapping);

    return kernel_mapping->vaddr;
}

void process_free(process_t * process) {
    heap_free(process);
}

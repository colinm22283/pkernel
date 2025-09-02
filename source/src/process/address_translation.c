#include <stddef.h>

#include <process/address_translation.h>

void * process_user_to_kernel(process_t * process, const void * user_vaddr) {
    pman_mapping_t * mapping = process->paging_context->head.next;

    while (mapping != &process->paging_context->tail) {
        if (
            user_vaddr >= mapping->vaddr &&
            user_vaddr < (void *) ((char *) mapping->vaddr + mapping->size_pages * 0x1000)
        ) {
            pman_mapping_t * root_mapping = get_root_mapping(mapping);

            return user_vaddr - mapping->vaddr + root_mapping->vaddr;
        }

        mapping = mapping->next;
    }

    return NULL;
}

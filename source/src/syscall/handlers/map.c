#include <process/scheduler.h>

#include <syscall/handlers/map.h>

void * syscall_map(fd_t fd, void * map_address, uint64_t size, uint64_t offset, map_options_t options) {
    if (size == 0) return NULL;

    process_t * current_process = scheduler_current_process();

    pman_context_t * context = current_process->paging_context;

    if (options & MAP_ANON) {
        pman_protection_flags_t prot = 0;
        if (options & MAP_WRITE) prot |= PMAN_PROT_WRITE;
        if (options & MAP_EXECUTE) prot |= PMAN_PROT_EXECUTE;

        pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, size);
        pman_mapping_t * user_mapping = pman_context_add_shared(context, prot, kernel_mapping, map_address);
        pman_context_unmap(kernel_mapping);

        return user_mapping->vaddr;
    }
    else {
        fs_file_t * file = process_file_table_get(&current_process->file_table, fd);

        return file_map(file, context, map_address, size, offset);
    }
}

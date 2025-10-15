#include <process/process.h>

#include <util/heap/heap.h>

process_t * process_create(void) {
    process_t * process = heap_alloc(sizeof(process_t));

    process->paging_context = pman_new_context();

    process->thread_count = 0;
    process->thread_capacity = 1;
    process->threads = heap_alloc(process->thread_capacity * sizeof(thread_t *));

    file_table_init(&process->file_table);

    process->working_dir = &fs_root;

    return process;
}

process_t * process_create_fork(process_t * parent) {
    process_t * process = process_create();

    return process;
}

void process_free(process_t * process) {
    file_table_free(&process->file_table);

    heap_free(process);
}

void process_add_thread(process_t * process, thread_t * thread) {
    process->threads[process->thread_count++] = thread;

    if (process->thread_count == process->thread_capacity) {
        process->thread_capacity *= 2;

        process->threads = heap_realloc(process->threads, process->thread_capacity * sizeof(thread_t *));
    }
}

void * process_create_segment(process_t * process, void * vaddr, size_t size, pman_protection_flags_t prot) {
    pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, size);
    pman_mapping_t * user_mapping = pman_context_add_shared(process->paging_context, prot, kernel_mapping, vaddr);
    pman_context_unmap(kernel_mapping);

    return kernel_mapping->vaddr;
}

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

fs_directory_entry_t * process_open_path(process_t * process, const char * path) {
    if (path[0] == '/') {
        if (path[1] == '\0') return &fs_root;
        else return fs_open_path(&fs_root, path + 1);
    }
    else {
        return fs_open_path(process->working_dir, path);
    }
}

fs_directory_entry_t * process_make_path(process_t * process, const char * path, fs_file_type_t type) {
    if (path[0] == '/') {
        if (path[1] == '\0') return &fs_root;
        else return fs_make_path(&fs_root, path + 1, type);
    }
    else return fs_make_path(process->working_dir, path, type);
}
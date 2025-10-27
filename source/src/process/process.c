#include <process/process.h>

#include <util/heap/heap.h>

#include <util/memory/memcpy.h>

#include <sys/push_args.h>

pid_t current_pid = 0;

process_t * process_create(void) {
    process_t * process = heap_alloc(sizeof(process_t));

    process->paging_context = pman_new_context();

    process->id = current_pid++;
    process->parent_id = 0;

    process->thread_count = 0;
    process->thread_capacity = 1;
    process->threads = heap_alloc(process->thread_capacity * sizeof(thread_t *));

    file_table_init(&process->file_table);

    process->working_dir = &fs_root;

    return process;
}

process_t * process_create_fork(process_t * parent) {
    process_t * process = process_create();

    process->parent_id = parent->id;

    for (pman_mapping_t * mapping = parent->paging_context->head.next; mapping != &parent->paging_context->tail; mapping = mapping->next) {
        if (mapping->protection & PMAN_PROT_SHARED) {
            vga_print("oh dear\n");
        }
        else {
            pman_context_add_borrowed(process->paging_context, mapping->protection, get_root_mapping(mapping), mapping->vaddr);
        }
    }

    thread_t * new_thread = thread_create_fork(process->paging_context, process, parent->threads[0]);

    process_add_thread(process, new_thread);

    file_table_clone(&process->file_table, &parent->file_table);

    fs_directory_entry_add_reference(parent->working_dir);
    process->working_dir = parent->working_dir;

    return process;
}

void process_free(process_t * process) {
    file_table_free(&process->file_table);

    if (process->argc != 0) heap_free(process->argv);

    pman_free_context(process->paging_context);

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

void process_remap(process_t * process, pman_mapping_t * old_mapping, pman_mapping_t * new_mapping) {
    for (size_t i = 0; i < process->thread_count; i++) {
        if (process->threads[i]->stack_mapping == old_mapping) process->threads[i]->stack_mapping = new_mapping;
    }
}

void process_push_args(process_t * process, const char ** argv, uint64_t argc) {
    if (process->argc != 0) {
        heap_free(process->argv);
    }

    if (argc == 0) {
        process->argc = 0;
        process->argv = NULL;
    }
    else {
        process->argc = argc;
        process->argv = heap_alloc(argc * sizeof(const char *));

        uint64_t required_size = 0;
        for (uint64_t i = 0; i < argc; i++) {
            required_size += strlen(argv[i]) + 1;
        }

        pman_mapping_t * kern_mapping = pman_context_add_alloc(
            pman_kernel_context(),
            0,
            NULL,
            required_size
        );

        pman_mapping_t * user_mapping = pman_context_add_shared(
            process->paging_context,
            0,
            kern_mapping,
            NULL
        );

        char * kern_buf = kern_mapping->vaddr;
        char * user_buf = user_mapping->vaddr;

        uint64_t pos = 0;
        for (uint64_t i = 0; i < argc; i++) {
            uint64_t len = strlen(argv[i]);

            process->argv[i] = &user_buf[pos];

            memcpy(&kern_buf[pos], argv[i], len + 1);
            pos += len + 1;
        }

        pman_context_unmap(kern_mapping);
    }

    push_args(process, &process->threads[0]->tsr, process->threads[0]->stack_mapping, process->argc, process->argv);
}

void process_kill(process_t * process) {
    for (size_t i = 0; i < process->thread_count; i++) {
        process->threads[i]->process = NULL;
        process->threads[i]->state = TS_DEAD;
    }
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
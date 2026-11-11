#include <process/process.h>
#include <process/trampoline.h>

#include <filesystem/filesystem.h>

#include <pman/pman.h>
#include <pman/source.h>
#include <pman/mapping.h>

#include <util/heap/heap.h>

#include <util/memory/memcpy.h>

#include <debug/printf.h>

#include <scheduler/scheduler.h>
#include <sys/paging/page_size.h>
#include <sys/process/trampoline.h>
#include <sys/process/push_args.h>

pid_t current_pid = 0;

process_t process_head, process_tail;

void processes_init(void) {
    process_head.global_next = &process_tail;
    process_head.global_prev = NULL;
    process_tail.global_next = NULL;
    process_tail.global_prev = &process_head;

    /* kernel_trampoline_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, process_trampoline_size); */
    /* memcpy(kernel_trampoline_mapping->vaddr, process_trampoline, process_trampoline_size); */
}

process_t * process_create(void) {
    heap_check();

    process_t * process = heap_alloc_debug(sizeof(process_t), "process");

    process->paging_context = pman_new_context();

    process->id = current_pid++;
    process->parent_id = 0;

    process->thread_count = 0;
    process->thread_capacity = 1;
    process->threads = heap_alloc_debug(process->thread_capacity * sizeof(thread_t *), "process threads");

    file_table_init(&process->file_table);

    process->working_dir = &fs_root;

    process->child_finished = event_init();

    signal_table_init(&process->signal_table);

    process->argc = 0;

    process->global_next = process_head.global_next;
    process->global_prev = &process_head;
    process_head.global_next->global_prev = process;
    process_head.global_next = process;

    return process;
}

process_t * process_create_fork(process_t * parent) {
    process_t * process = process_create();

    process->parent_id = parent->id;

    pman_fork_context(process->paging_context, parent->paging_context);

    thread_t * new_thread = thread_create_fork(process->paging_context, process, parent->threads[0]);

    process_add_thread(process, new_thread);

    file_table_clone(&process->file_table, &parent->file_table);

    fs_directory_entry_add_reference(parent->working_dir);
    process->working_dir = parent->working_dir;

    return process;
}

void process_free(process_t * process) {
    process->global_next->global_prev = process->global_prev;
    process->global_prev->global_next = process->global_next;

    process_t * parent = process_lookup(process->parent_id);

    if (parent != NULL) {
        signal_table_invoke(parent, SIG_CHILD);

        event_invoke(parent->child_finished);
    }

    for (size_t i = 0; i < process->thread_count; i++) {
        process->threads[i]->process = NULL;

        process->threads[i]->state = TS_DEAD;
    }

    signal_table_free(&process->signal_table);

    event_free(process->child_finished);

    file_table_free(&process->file_table);

    if (process->argc != 0) heap_free(process->argv);

    fs_directory_entry_release(process->working_dir);

    pman_free_context(process->paging_context);

    heap_free(process->threads);

    heap_free(process);
}

process_t * process_lookup(pid_t pid) {
    for (process_t * process = process_head.global_next; process != &process_tail; process = process->global_next) {
        if (process->id == pid) return process;
    }

    return NULL;
}

void process_add_thread(process_t * process, thread_t * thread) {
    process->threads[process->thread_count++] = thread;

    if (process->thread_count == process->thread_capacity) {
        process->thread_capacity *= 2;

        process->threads = heap_realloc(process->threads, process->thread_capacity * sizeof(thread_t *));
    }
}

long process_copy_from_user(process_t * process, void * dst, const void * src, long size) {
    pman_mapping_t * mapping = pman_get_mapping(process->paging_context, src);
    if (mapping == NULL) return -EFAULT;
    
    int offset = (intptr_t) src % PAGE_SIZE;

    for (long i = 0; i < size; i++) {
        char c = ((const char *) pman_source_map_to_kernel(mapping->source))[offset++];
        ((char *) dst)[i] = c;

        if (offset == PAGE_SIZE) {
            mapping = pman_get_mapping(process->paging_context, (void *) ((intptr_t) src + i));
            if (mapping == NULL) return -EFAULT;

            offset = 0;
        }
    }

    return size;
}

long process_strncpy_from_user(process_t * process, void * dst, const void * src, long size) {
    pman_mapping_t * mapping = pman_get_mapping(process->paging_context, src);
    if (mapping == NULL) return -EFAULT;
    const char * kvaddr = pman_source_map_to_kernel(mapping->source);

    int offset = (intptr_t) src % PAGE_SIZE;

    for (long i = 0; i < size - 1; i++) {
        char c = kvaddr[offset++];
        ((char *) dst)[i] = c;

        if (c == '\0') return i;

        if (offset == PAGE_SIZE) {
            mapping = pman_get_mapping(process->paging_context, src);
            if (mapping == NULL) return -EFAULT;
            kvaddr = pman_source_map_to_kernel(mapping->source);

            offset = 0;
        }
    }

    ((char *) dst)[size] = '\0';

    return size;
}

long process_copy_to_user(process_t * process, void * dst, const void * src, long size) {
    pman_range_t * range = pman_get_range(process->paging_context, dst, size);

    if (range == NULL) {
        return 0;
    }

    if (!range->complete) {
        pman_range_free(range);
        return 0;
    }

    size_t offset = ((intptr_t) dst) % PAGE_SIZE;

    pman_mapping_t ** mapping = &range->mappings[0];
    pman_mapping_prepare_write(*mapping);
    char * kvaddr = pman_source_map_to_kernel((*mapping)->source);

    for (long i = 0; i < size; i++) {
        if (kvaddr == NULL) {
            return i;
        }

        kvaddr[offset++] = ((char *) src)[i];

        if (offset == PAGE_SIZE) {
            mapping++;
            pman_mapping_prepare_write(*mapping);
            kvaddr = pman_source_map_to_kernel((*mapping)->source);
            offset = 0;
        }
    }

    return size;
}

pman_range_t * process_create_segment(process_t * process, void * vaddr, size_t size, pman_protection_flags_t prot) {
    return pman_add_anon_map(
        process->paging_context,
        vaddr,
        size,
        0,
        prot
    );
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
        process->argv = heap_alloc_debug(argc * sizeof(const char *), "process argv");

        uint64_t required_size = 0;
        for (uint64_t i = 0; i < argc; i++) {
            required_size += strlen(argv[i]) + 1;
        }

        pman_range_t * arg_range = pman_add_anon_map(
            process->paging_context,
            NULL,
            required_size,
            0,
            0
        );

        char * user_vaddr = arg_range->vaddr;

        size_t pos = 0;
        for (uint64_t i = 0; i < argc; i++) {
            uint64_t len = strlen(argv[i]);

            process->argv[i] = &user_vaddr[pos];

            process_copy_to_user(process, &user_vaddr[pos], argv[i], len + 1);
            pos += len + 1;
        }
    }

    push_main_args(process, &process->threads[0]->tsr, process->argc, process->argv);
}

void process_set_working_dir(process_t * process, fs_directory_entry_t * dirent) {
    fs_directory_entry_release(process->working_dir);

    process->working_dir = dirent;
}

void process_kill(process_t * process) {
    for (size_t i = 0; i < process->thread_count; i++) {
        process->threads[i]->process = NULL;
        thread_kill(process->threads[i]);
    }

    process_free(process);
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

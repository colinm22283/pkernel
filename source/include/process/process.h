#pragma once

#include <stddef.h>

#include <process/thread.h>
#include <process/file_table.h>

#include <paging/manager.h>

#include <signal/signal.h>

typedef struct process_s {
    pman_context_t * paging_context;

    pid_t id;
    pid_t parent_id;

    size_t thread_count;
    size_t thread_capacity;
    thread_t ** threads;

    file_table_t file_table;

    fs_directory_entry_t * working_dir;

    uint64_t argc;
    char ** argv;

    event_t * child_finished;

    signal_table_t signal_table;

    struct process_s * next;
    struct process_s * prev;

    struct process_s * global_next;
    struct process_s * global_prev;
} process_t;

extern process_t process_head, process_tail;

void processes_init(void);

process_t * process_create(void);
process_t * process_create_fork(process_t * parent);

void process_free(process_t * process);

process_t * process_lookup(pid_t pid);

void process_add_thread(process_t * process, thread_t * thread);

void * process_create_segment(process_t * process, void * vaddr, size_t size, pman_protection_flags_t prot);

void * process_user_to_kernel(process_t * process, const void * user_vaddr);

void process_remap(process_t * process, pman_mapping_t * old_mapping, pman_mapping_t * new_mapping);

void process_push_args(process_t * process, const char ** argv, uint64_t argc);

void process_set_working_dir(process_t * process, fs_directory_entry_t * dirent);

void process_kill(process_t * process);

fs_directory_entry_t * process_open_path(process_t * process, const char * path);
fs_directory_entry_t * process_make_path(process_t * process, const char * path, fs_file_type_t type);

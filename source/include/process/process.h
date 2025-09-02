#pragma once

#include <stdint.h>

#include <paging/manager.h>

#include <filesystem/file.h>
#include <filesystem/directory_entry.h>

#include <process/file_table.h>
#include <process/thread_table.h>
#include <process/signal.h>

#include <interrupt/interrupt_state_record.h>

typedef uint64_t process_id_t;

typedef enum {
    PS_STOPPED,
    PS_RUNNING,
    PS_DEAD,
} process_state_t;

typedef struct process_s {
    process_state_t state;
    process_id_t id, parent_id;

    uint64_t text_size;
    uint64_t data_size;
    uint64_t rodata_size;
    uint64_t bss_size;
    uint64_t stack_size;

    pman_context_t * paging_context;
    pman_mapping_t * text;
    pman_mapping_t * data;
    pman_mapping_t * rodata;
    pman_mapping_t * bss;

    process_file_table_t file_table;

    process_thread_table_t thread_table;

    signal_table_t signal_table;

    fs_directory_entry_t * working_dir;

    const char ** argv;
    uint64_t argc;

    struct process_s * next;
    struct process_s * prev;
} process_t;

void process_init(
    process_t * process,
    process_id_t parent_id,
    process_id_t id,
    uint64_t text_size,
    uint64_t data_size,
    uint64_t rodata_size,
    uint64_t bss_size,
    uint64_t stack_size
);
void process_init_fork(
    process_t * process,
    process_t * parent,
    process_id_t id
);
void process_free(process_t * process);

void process_push_args(process_t * process, const char ** argv, uint64_t argc);

void process_load_isr(process_t * process, interrupt_state_record_t * isr);

void process_set_working_dir(process_t * process, fs_directory_entry_t * directory_entry);

void process_start(process_t * process);
void process_kill(process_t * process);

fs_directory_entry_t * process_open_path(process_t * process, const char * path);
fs_directory_entry_t * process_make_path(process_t * process, const char * path, fs_file_type_t type);


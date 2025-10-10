#pragma once

#include <stdint.h>

#include <paging/manager.h>

#include <sys/isr/isr.h>

#include <pkos/types.h>

struct process_s;

typedef enum {
    TS_RUNNING,
    TS_WAIT_CHILD,
    TS_WAIT_IO,
    TS_WAIT_EVENT,
} process_thread_state_t;

typedef union {
    struct {

    } child;

    struct {
        fd_t file_fd;

        char * buffer;
        uint64_t size;
    } io;

    struct {
        struct event_waiter_s * waiter;
    } event;
} process_thread_wait_info_t;

typedef struct process_thread_s {
    process_thread_state_t state;

    process_thread_wait_info_t wait_info;

    interrupt_state_record_t isr;

    pman_mapping_t * kernel_stack;
    pman_mapping_t * stack;
} process_thread_t;

typedef struct {
    struct process_s * process;

    uint64_t current_index;

    uint64_t thread_count, thread_capacity;
    process_thread_t ** threads;
} process_thread_table_t;

void process_thread_table_init(process_thread_table_t * table, struct process_s * process);
void process_thread_table_free(process_thread_table_t * table);

process_thread_t * process_thread_table_create_thread(process_thread_table_t * table, uint64_t stack_size);
process_thread_t * process_thread_table_create_thread_fork(process_thread_table_t * table, struct process_s * parent_process, process_thread_t * parent_thread);


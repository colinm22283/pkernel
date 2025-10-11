#pragma once

#include <stddef.h>

#include <process/thread.h>

#include <paging/manager.h>

typedef struct process_s {
    pman_context_t * paging_context;

    size_t thread_count;
    thread_t ** threads;

    struct process_s * next;
    struct process_s * prev;
} process_t;

process_t * process_create(void);
process_t * process_create_fork(process_t * parent);

void * process_create_segment(process_t * process, void * vaddr, size_t size, pman_protection_flags_t prot);

void process_free(process_t * process);

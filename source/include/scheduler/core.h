#pragma once

#include <stddef.h>

#include <process/process.h>

#include <paging/manager.h>

typedef struct {
    pman_context_t * paging_context;

    process_t * current_process;
    thread_t * current_thread;
} scheduler_core_t;

extern size_t core_count;
extern scheduler_core_t * cores;

void scheduler_cores_init(void);
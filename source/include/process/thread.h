#pragma once

#include <sys/tsr/tsr.h>

struct process_s;

typedef enum {
    TL_USER,
    TL_KERNEL,
} thread_level_t;

typedef enum {
    TS_RUNNING,
    TS_WAITING,
} thread_state_t;

typedef struct thread_s {
    struct process_s * process;

    thread_level_t level;
    thread_state_t state;

    task_state_record_t isr;

    struct thread_s * twin_thread;

    struct thread_s * next;
    struct thread_s * prev;
} thread_t;

__NORETURN void thread_resume(thread_t * thread);
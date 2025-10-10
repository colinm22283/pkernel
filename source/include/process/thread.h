#pragma once

struct process_s;

typedef enum {
    PL_USER,
    PL_KERNEL,
} thread_level_t;

typedef struct thread_s {
    struct process_s * process;
} thread_t;
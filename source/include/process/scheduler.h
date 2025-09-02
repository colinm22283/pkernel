#pragma once

#include <process/process.h>

#include <defs.h>

void scheduler_init(void);

process_t * scheduler_queue(
    process_id_t parent_id,
    uint64_t text_size,
    uint64_t data_size,
    uint64_t rodata_size,
    uint64_t bss_size,
    uint64_t stack_size
);

process_t * scheduler_queue_fork(
    process_t * parent
);

process_t * scheduler_get_id(process_id_t id);

__NORETURN void scheduler_start(void);

process_t * scheduler_current_process(void);
process_thread_t * scheduler_current_thread(void);

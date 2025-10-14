#pragma once

#include <sys/tsr/tsr_load_pc.h>

static inline void tsr_load_task(task_state_record_t * tsr, void (*task_handler)(task_state_record_t * tsr)) {
    tsr->rdi = (uint64_t) tsr;

    tsr_load_pc(tsr, task_handler);
}

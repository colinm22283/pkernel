#pragma once

#include <sys/tsr/tsr_load_pc.h>

static inline void tsr_load_task(task_state_record_t * tsr, task_state_record_t * user_tsr, void (*task_handler)(task_state_record_t * tsr)) {
    tsr->rdi = (uint64_t) user_tsr;

    tsr_load_pc(tsr, task_handler);
}

#pragma once

static inline void pop_tsr(process_t * process, task_state_record_t * tsr) {
    process_copy_from_user(process, tsr, (void *) tsr->rsp, sizeof(task_state_record_t));
}

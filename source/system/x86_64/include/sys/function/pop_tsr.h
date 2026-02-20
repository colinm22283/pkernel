#pragma once

static inline void pop_tsr(process_t * process, task_state_record_t * tsr) {
    char * kern_rsp = process_user_to_kernel(
        process,
        (void *) tsr->rsp
    );
    memcpy(tsr, (task_state_record_t *) kern_rsp, sizeof(task_state_record_t));
}
#pragma once

#include <util/memory/memcpy.h>

static inline void push_tsr(process_t * process, task_state_record_t * tsr) {
    char * kern_rsp = process_user_to_kernel(
        process,
        (void *) tsr->rsp
    );
    kern_rsp -= sizeof(task_state_record_t);
    memcpy((task_state_record_t *) kern_rsp, tsr, sizeof(task_state_record_t));

    tsr->rsp -= sizeof(task_state_record_t);
}
#pragma once

#include <util/memory/memcpy.h>

static inline void push_tsr(process_t * process, task_state_record_t * tsr) {
    process_copy_to_user(process, (void *) (tsr->rsp - sizeof(task_state_record_t)), tsr, sizeof(task_state_record_t));

    tsr->rsp -= sizeof(task_state_record_t);
}

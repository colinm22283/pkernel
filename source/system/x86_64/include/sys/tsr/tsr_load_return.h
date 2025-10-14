#pragma once

#include <sys/tsr/tsr.h>

static inline void tsr_load_return(task_state_record_t * tsr, uint64_t value) {
    tsr->rax = value;
}
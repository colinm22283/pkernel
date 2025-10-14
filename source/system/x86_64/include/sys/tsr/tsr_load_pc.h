#pragma once

#include <sys/tsr/tsr.h>

static inline void tsr_load_pc(task_state_record_t * tsr, void * pc) {
    tsr->rip = (uint64_t) pc;
}

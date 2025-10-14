#pragma once

#include <sys/tsr/tsr.h>

static inline void tsr_set_stack(task_state_record_t * tsr, void * stack, uint64_t size) {
    tsr->rbp = tsr->rsp = (intptr_t) stack + size - sizeof(uint64_t);
}

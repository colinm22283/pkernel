#pragma once

#include <stdbool.h>

#include <sys/tsr/tsr.h>

void _store_tsr_and_yield(task_state_record_t * isr);

static inline void store_tsr_and_yield(task_state_record_t * isr) {
    _store_tsr_and_yield(isr);
}
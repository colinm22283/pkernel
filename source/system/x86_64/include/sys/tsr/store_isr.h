#pragma once

#include <sys/tsr/tsr.h>

void _store_isr(interrupt_state_record_t * isr);

static inline void store_isr(interrupt_state_record_t * isr) {
    _store_isr(isr);
}
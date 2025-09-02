#pragma once

#include <interrupt/interrupt_state_record.h>

struct process_thread_s;

typedef struct {
    struct process_thread_s * thread;

    interrupt_state_record_t isr;
} event_waiter_t;

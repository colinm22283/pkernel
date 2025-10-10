#pragma once

#include <paging/manager.h>

#include <sys/isr/isr.h>

#include <defs.h>

typedef struct event_waiter_s {
    struct event_s * event;

    pman_mapping_t * stack_mapping;

    interrupt_state_record_t isr;

    struct event_waiter_s * next;
    struct event_waiter_s * prev;
} event_waiter_t;

__NORETURN void event_waiter_resume_and_free(event_waiter_t * waiter);

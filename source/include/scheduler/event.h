#pragma once

#include <process/thread.h>

typedef struct waiter_s {
    thread_t * thread;

    struct waiter_s * next;
    struct waiter_s * prev;
} waiter_t;

typedef struct event_s {
    bool has_signal;

    waiter_t waiter_head, waiter_tail;
} event_t;

event_t * event_init(void);
void event_free(event_t * event);

void event_invoke(event_t * event);

void waiter_free(waiter_t * waiter);

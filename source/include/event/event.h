#pragma once

#include <event/waiter.h>

#include <_process/thread_table.h>

#include <error_number.h>

#include <defs.h>

typedef struct event_s {
    event_waiter_t head, tail;

    struct event_s * next;
    struct event_s * prev;
} event_t;

event_t * event_init(void);
error_number_t event_free(event_t * event);

void event_await(process_thread_t * process, event_t * event);

void event_invoke_once(event_t * event);

void event_manager_init(void);
void event_manager_resume(void);

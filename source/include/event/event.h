#pragma once

#include <event/waiter.h>

#include <error_number.h>

typedef struct {
    uint64_t waiter_count;
    event_waiter_t ** waiters;
} event_t;

event_t * event_init(void);
error_number_t event_free(event_t * event);

event_waiter_t * event_await(event_t * event);

void event_invoke_once(event_t * event);

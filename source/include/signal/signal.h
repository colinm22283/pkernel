#pragma once

#include <pkos/signal.h>
#include <error_number.h>

typedef enum {
    ACT_TERMINATE,
    ACT_IGNORE,
    ACT_STOP,
    ACT_CONTINUE,
} signal_action_t;

typedef struct {
    signal_action_t default_action;

    void * user_handler;
} signal_t;

typedef struct {
    signal_t handlers[_SIG_COUNT];
} signal_table_t;

void signal_table_init(signal_table_t * st);
void signal_table_free(signal_table_t * st);

error_number_t signal_table_invoke(signal_table_t * st, signal_number_t sig, thread_t * handling_thread);

error_number_t signal_table_set(signal_table_t * st, signal_number_t sig, signal_handler_t * handler);

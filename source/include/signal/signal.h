#pragma once

#include <pkos/signal.h>
#include <error_number.h>

#include <process/thread.h>

#include <config/signal.h>

typedef enum {
    ACT_TERMINATE,
    ACT_IGNORE,
    ACT_STOP,
    ACT_CONTINUE,
} signal_action_t;

typedef struct {
    signal_action_t action;

    void * user_handler;
} signal_t;

typedef struct {
    signal_t handlers[_SIG_COUNT];

    size_t signal_queue_size, signal_queue_head, signal_queue_tail;
    signal_number_t signal_queue[SIGNAL_QUEUE_SIZE];
} signal_table_t;

extern signal_action_t signal_defaults[_SIG_COUNT];

void signal_table_init(signal_table_t * st);
void signal_table_free(signal_table_t * st);

error_number_t signal_table_invoke(struct process_s * process, signal_number_t sig);

error_number_t signal_table_set(signal_table_t * st, signal_number_t sig, signal_handler_t * handler);
error_number_t signal_table_reset(signal_table_t * st, signal_number_t sig);

error_number_t signal_table_resume(struct thread_s * thread);

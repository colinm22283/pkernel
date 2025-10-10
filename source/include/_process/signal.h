#pragma once

#include <pkos/signal.h>

struct process_s;

typedef struct signal_table_signal_s {
    signal_number_t number;
    void * cookie;

    struct signal_table_signal_s * next;
    struct signal_table_signal_s * prev;
} signal_table_signal_t;

typedef struct {
    uint64_t signal_rip;
} signal_table_handler_t;

typedef struct {
    signal_table_signal_t head, tail;

    uint64_t handler_count, handler_capacity;
    signal_table_handler_t * handlers;
} signal_table_t;

void signal_table_init(signal_table_t * st);
void signal_table_free(signal_table_t * st);

void signal_table_register(signal_table_t * st, uint64_t rip);

void signal_table_invoke(signal_table_t * st, signal_number_t number, void * cookie);

void signal_table_enter(signal_table_t * st, struct process_s * process);

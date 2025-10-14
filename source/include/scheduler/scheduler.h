#pragma once

#include <process/process.h>

#include <scheduler/event.h>

#include <defs.h>

void scheduler_init(void);

void scheduler_queue(thread_t * thread);

void scheduler_await(event_t * event);

void scheduler_load_tsr(task_state_record_t * tsr);

void scheduler_start_twin(void (*task_handler)(task_state_record_t * tsr));
__NORETURN void scheduler_return_twin(uint64_t ret_val);

__NORETURN void scheduler_yield(void);

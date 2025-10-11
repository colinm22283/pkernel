#pragma once

#include <process/process.h>

#include <defs.h>

void scheduler_init(void);

void scheduler_queue(thread_t * thread);

__NORETURN void scheduler_yield(void);

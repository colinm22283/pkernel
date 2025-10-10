#pragma once

#include <process/process.h>

#include <defs.h>

typedef enum {
    SP_CRITICAL,
    SP_HIGH,
    SP_MEDIUM,
    SP_LOW,

    SP_COUNT,
} scheduler_priority_t;

void scheduler_init(void);

__NORETURN void scheduler_yield(void);

#pragma once

#include <stddef.h>

typedef struct {

} scheduler_core_t;

extern size_t core_count;
extern scheduler_core_t * cores;

void scheduler_cores_init(void);
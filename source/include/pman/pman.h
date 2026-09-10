#pragma once

#include <pman/types.h>

pman_init(void);

pman_context_t * pman_next_context(void);
int pman_free_context(pman_context_t * context);

void pman_context_load_table(pman_context_t * context);

static inline pman_context_t * pman_kernel_context(void) {
    extern pman_context_t kernel_context;

    return &kernel_context;
}


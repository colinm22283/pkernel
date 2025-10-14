#pragma once

#include <stdbool.h>

#include <sys/tsr/tsr.h>

typedef enum {
    IC_CRITICAL_ERROR,
    IC_DIV0,
    IC_FPU_ERROR,
    IC_INVALID_OPCODE,
    IC_PAGE_FAULT,
    IC_TIMER,
    IC_SYSCALL,

    IC_CUSTOM_BASE,
} interrupt_code_t;

typedef void (interrupt_handler_t)(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code);

void interrupt_registry_init(void);

bool interrupt_registry_register(interrupt_code_t channel, interrupt_handler_t * handler);
bool interrupt_registry_free(interrupt_code_t channel);

void interrupt_registry_invoke(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code);
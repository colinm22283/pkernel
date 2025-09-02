#pragma once

#include <stdbool.h>

typedef enum {
    IC_DIV0,
    IC_NMI,
    IC_BP_INT3,
    IC_OVF,
    IC_BOUND_RANGE,
    IC_INVALID_OPCODE,
    IC_DEVICE_NOT_AVAIL,
    IC_DOUBLE_FAULT,
    IC_COPROC_SEGMENT_OVERRUN,
    IC_INVALID_TSS,
    IC_SEGMENT_NOT_PRESENT,
    IC_STACK_SEGMENT_FAULT,
    IC_GENERAL_PROTECTION_FAULT,
    IC_X87_FPU,
    IC_ALIGNMENT_CHECK,
    IC_MACHINE_CHECK,
    IC_SIMD_FPU_ERROR,

    IC_TIMER,
    IC_KEYBOARD,

    IC_LENGTH,
} interrupt_channel_t;

typedef bool (* interrupt_handler_t)(interrupt_channel_t channel, void * cookie);

void interrupt_registry_init(void);

bool interrupt_registry_register(interrupt_channel_t channel, interrupt_handler_t handler, void * cookie);
bool interrupt_registry_free(interrupt_channel_t channel);

void interrupt_registry_invoke(interrupt_channel_t channel);
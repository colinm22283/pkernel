#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint64_t
        rax,
        rbx,
        rcx,
        rdx,
        rsi,
        rdi,
        rbp,
        rsp,
        r8,
        r9,
        r10,
        r11,
        r12,
        r13,
        r14,
        r15,
        rip,
        flags;
} interrupt_state_record_t;
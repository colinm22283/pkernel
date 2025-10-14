#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint64_t
        rax, // 0
        rbx, // 8
        rcx, // 16
        rdx, // 24
        rsi, // 32
        rdi, // 40
        rbp, // 48
        rsp, // 56
        r8,  // 64
        r9,  // 72
        r10, // 80
        r11, // 88
        r12, // 96
        r13, // 104
        r14, // 112
        r15, // 120
        rip, // 128
        flags; // 136
} task_state_record_t;

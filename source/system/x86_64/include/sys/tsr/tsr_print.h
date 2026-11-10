#pragma once

#include <sys/tsr/tsr.h>

#include <debug/printf.h>

static inline void tsr_print(task_state_record_t * tsr) {
    printf("rax = %p\n", tsr->rax);
    printf("rbx = %p\n", tsr->rbx);
    printf("rcx = %p\n", tsr->rcx);
    printf("rdx = %p\n", tsr->rdx);
    printf("rsi = %p\n", tsr->rsi);
    printf("rdi = %p\n", tsr->rdi);
    printf("rbp = %p\n", tsr->rbp);
    printf("rsp = %p\n", tsr->rsp);
    printf("r8 = %p\n", tsr->r8);
    printf("r9 = %p\n", tsr->r9);
    printf("r10 = %p\n", tsr->r10);
    printf("r11 = %p\n", tsr->r11);
    printf("r12 = %p\n", tsr->r12);
    printf("r13 = %p\n", tsr->r13);
    printf("r14 = %p\n", tsr->r14);
    printf("r15 = %p\n", tsr->r15);
    printf("rip = %p\n", tsr->rip);
    printf("flags = %p\n", tsr->flags);
}


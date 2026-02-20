#pragma once

#include <process/process.h>

#include <paging/manager.h>

#include <sys/tsr/tsr.h>
#include <sys/function/arg.h>

static inline void push_main_args(process_t * process, task_state_record_t * tsr, pman_mapping_t * stack_mapping, uint64_t argc, char ** argv) {
    uint64_t * kern_rsp = process_user_to_kernel(
        process,
        (void *) tsr->rsp
    );

    kern_rsp -= argc;

    for (uint64_t i = 0; i < argc; i++) {
        kern_rsp[i] = (uint64_t) argv[i];
    }
    kern_rsp--;
    *kern_rsp = argc;

    tsr->rsp -= sizeof(uint64_t) * (1 + argc);
}

static inline void push_args(process_t * process, task_state_record_t * tsr, arg_t * argv, size_t argc) {
    if (argc >= 1) {
        tsr->rdi = argv[0];
    }
    if (argc >= 2) {
        tsr->rsi = argv[1];
    }
    if (argc >= 3) {
        tsr->rdx = argv[2];
    }
    if (argc >= 4) {
        tsr->rcx = argv[3];
    }
    if (argc >= 5) {
        tsr->r8 = argv[4];
    }
}
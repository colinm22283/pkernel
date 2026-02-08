#pragma once

#include <process/process.h>

#include <sys/function/push_args.h>

static inline void push_function(process_t * process, task_state_record_t * tsr, void * addr, arg_t * argv, size_t argc) {
    uint64_t * kern_rsp = process_user_to_kernel(
        process,
        (void *) tsr->rsp
    );
    kern_rsp--;
    kern_rsp[0] = tsr->rip;
    tsr->rsp -= sizeof(uint64_t);

    tsr->rsp = (intptr_t) addr;

    push_args(process, tsr, argv, argc);

    tsr->rip = (uint64_t) addr;
}
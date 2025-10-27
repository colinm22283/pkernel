#pragma once

#include <process/process.h>

#include <paging/manager.h>

#include <sys/tsr/tsr.h>

static inline void push_args(process_t * process, task_state_record_t * tsr, pman_mapping_t * stack_mapping, uint64_t argc, char ** argv) {
    uint64_t * kern_rsp = process_user_to_kernel(
        process,
        (void *) tsr->rsp
    );

    kern_rsp -= argc;

    for (uint64_t i = 0; i < process->argc; i++) {
        kern_rsp[i] = (uint64_t) process->argv[i];
    }
    kern_rsp--;
    *kern_rsp = process->argc;

    tsr->rsp -= sizeof(uint64_t) * (1 + argc);
}
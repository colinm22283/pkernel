#pragma once

#include <process/process.h>

#include <pman/pman.h>

#include <util/heap/heap.h>

#include <sys/tsr/tsr.h>
#include <sys/function/arg.h>

static inline void push_main_args(process_t * process, task_state_record_t * tsr, uint64_t argc, char ** argv) {
    uint64_t * stack = heap_alloc((argc + 1) * sizeof(uint64_t));
    stack[0] = argc;
    for (uint64_t i = 0; i < argc; i++) {
        stack[i + 1] = (uint64_t) argv[i];
    }

    tsr->rsp -= sizeof(uint64_t) * (1 + argc);
    process_copy_to_user(process, (void *) tsr->rsp, stack, (argc + 1) * sizeof(uint64_t));

    heap_free(stack);
}


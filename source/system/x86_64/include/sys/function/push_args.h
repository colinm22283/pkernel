#pragma once

#include <process/process.h>

#include <pman/pman.h>

#include <util/heap/heap.h>

#include <sys/tsr/tsr.h>
#include <sys/function/arg.h>


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



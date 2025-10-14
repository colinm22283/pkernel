#pragma once

#include <stdint.h>

#include <sys/tsr/tsr.h>

uint64_t syscall_handler(
    uint64_t syscall_number,
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2,
    uint64_t arg3,
    uint64_t arg4,
    uint64_t arg5,
    task_state_record_t * tsr
);
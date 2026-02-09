#include <scheduler/scheduler.h>

#include <syscall/handlers/exit.h>

#include <util/heap/heap.h>

void syscall_exit(__MAYBE_UNUSED uint64_t exit_code) {
    process_t * current_process = scheduler_current_process();

    process_kill(current_process);
}

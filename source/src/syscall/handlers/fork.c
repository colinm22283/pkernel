#include <scheduler/scheduler.h>

#include <syscall/handlers/fork.h>

#include <sys/tsr/tsr_load_return.h>

#include <sys/debug/print.h>

uint64_t syscall_fork(void) {
    process_t * current_process = scheduler_current_process();

    process_t * child = process_create_fork(current_process);

    tsr_load_return(&child->threads[0]->tsr, 0);

    thread_run(child->threads[0]);

    return child->id;
}

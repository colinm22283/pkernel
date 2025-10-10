#include <_process/scheduler.h>

#include <syscall/handlers/fork.h>

#include <debug/vga_print.h>

uint64_t syscall_fork(void) {
    process_t * current_process = scheduler_current_process();

    process_t * child = scheduler_queue_fork(current_process);

    child->thread_table.threads[0]->isr.rax = 0;

    process_start(child);

    return child->id;
}

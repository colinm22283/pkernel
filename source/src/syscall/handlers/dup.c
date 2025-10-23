#include <scheduler/scheduler.h>

#include <syscall/handlers/dup.h>

int64_t syscall_dup(fd_t dst, fd_t src) {
    process_t * current_process = scheduler_current_process();

    return file_table_dup(&current_process->file_table, dst, src);
}
#include <_process/scheduler.h>

#include <syscall/handlers/close.h>

int64_t syscall_close(fd_t fd) {
    process_t * current_process = scheduler_current_process();

    return process_file_table_close(&current_process->file_table, fd);
}
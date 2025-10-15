#include <stddef.h>

#include <syscall/handlers/write.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

int64_t syscall_write(fd_t fd, const char * _buffer, uint64_t size) {
    process_t * current_process = scheduler_current_process();

    const char * buffer = process_user_to_kernel(current_process, _buffer);
    if (buffer == NULL) return ERROR_BAD_PTR;

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) return ERROR_BAD_FD;

    return file_write(file, buffer, size);
}

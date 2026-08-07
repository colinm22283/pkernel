#include <stddef.h>

#include <syscall/handlers/readdir.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

int syscall_readdir(fd_t fd, struct dirent * _entries, size_t size) {
    process_t * current_process = scheduler_current_process();

    struct dirent * entries = process_user_to_kernel(current_process, _entries);
    if (entries == NULL) return -EFAULT;

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) return -EBADF;

    return file_readdir(file, entries, size);
}

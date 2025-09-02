#include <process/scheduler.h>

#include <syscall/handlers/dup.h>

int64_t syscall_dup(fd_t dst, fd_t src) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = process_file_table_get(&current_process->file_table, src);
    if (file == NULL) return ERROR_BAD_FD;

    fs_directory_entry_add_reference(file->dirent);

    process_file_table_set(&current_process->file_table, dst, file->dirent, file->options);

    return ERROR_OK;
}
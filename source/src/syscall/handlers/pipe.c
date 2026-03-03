#include <stddef.h>
#include <fcntl.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/pipe.h>

int64_t syscall_pipe(fd_t * _fds, int options) {
    process_t * current_process = scheduler_current_process();

    fd_t * fds = process_user_to_kernel(current_process, _fds);
    if (fds == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * dirent = fs_make_anon_pipe();
    if (dirent == NULL) return ERROR_UNKNOWN;

    fds[0] = file_table_open(&current_process->file_table, dirent, O_WRONLY);
    fds[1] = file_table_open(&current_process->file_table, dirent, O_RDONLY);

    return 0;
}


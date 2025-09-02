#include <stddef.h>

#include <process/scheduler.h>

#include <process/address_translation.h>

#include <syscall/handlers/pipe.h>

#include <debug/vga_print.h>

int64_t syscall_pipe(fd_t * _fds, open_options_t options) {
    process_t * current_process = scheduler_current_process();

    fd_t * fds = process_user_to_kernel(current_process, _fds);
    if (fds == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * dirent = fs_make_anon(FS_PIPE);
    if (dirent == NULL) return ERROR_UNKNOWN;

    fds[0] = process_file_table_open(&current_process->file_table, dirent, OPEN_WRITE);
    fds[1] = process_file_table_open(&current_process->file_table, dirent, OPEN_READ);

    return ERROR_OK;
}


#include <stddef.h>

#include <scheduler/scheduler.h>

#include <syscall/handlers/open.h>

#include <debug/vga_print.h>

int64_t syscall_open(const char * _path, open_options_t options) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, (char *) _path);
    if (path == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) {
        if (options & OPEN_CREATE) {

            node = process_make_path(current_process, path, FS_REGULAR);
        }
        else return ERROR_FS_NO_ENT;
    }

    fd_t fd = file_table_open(&current_process->file_table, node, options);

    return fd;
}

int64_t syscall_openat(fd_t fd, const char * _path, open_options_t options) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, (char *) _path);
    if (path == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) {
        if (options & OPEN_CREATE) {

            node = process_make_path(current_process, path, FS_REGULAR);
        }
        else return ERROR_FS_NO_ENT;
    }

    return file_table_openat(&current_process->file_table, fd, node, options);
}

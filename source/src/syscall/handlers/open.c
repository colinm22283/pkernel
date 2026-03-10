#include <stddef.h>
#include <fcntl.h>

#include <scheduler/scheduler.h>

#include <syscall/handlers/open.h>

int64_t syscall_open(const char * _path, int options) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, (char *) _path);
    if (path == NULL) return -EFAULT;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) {
        if (options & O_CREAT) {
            node = process_make_path(current_process, path, FS_REGULAR);
        }
        else return -ENOENT;
    }

    fd_t fd = file_table_open(&current_process->file_table, node, options);

    return fd;
}

int64_t syscall_openat(fd_t fd, const char * _path, int options) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, (char *) _path);
    if (path == NULL) return -EFAULT;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) {
        if (options & O_CREAT) {
            node = process_make_path(current_process, path, FS_REGULAR);
        }
        else return -ENOENT;
    }

    return file_table_openat(&current_process->file_table, fd, node, options);
}

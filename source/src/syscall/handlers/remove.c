#include <scheduler/scheduler.h>

#include <syscall/handlers/remove.h>

int syscall_remove(const char * _path) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, _path);
    if (path == NULL) return -EFAULT;

    fs_directory_entry_t * dirent = process_open_path(current_process, path);
    if (dirent == NULL) return -ENOENT;

    return fs_remove(dirent);
}

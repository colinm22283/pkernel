#include <scheduler/scheduler.h>

#include <syscall/handlers/unmount.h>

int syscall_unmount(const char * _path) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, _path);
    if (path == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * mount_point = process_open_path(current_process, path);
    if (mount_point == NULL) return ERROR_FS_NO_ENT;

    return fs_unmount(mount_point);
}
#include <scheduler/scheduler.h>

#include <syscall/handlers/mkdir.h>

#include <errno.h>

int syscall_mkdir(const char * _path) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, _path);
    if (path == NULL) return -EINVAL;

    fs_directory_entry_t * new_dirent = process_make_path(current_process, path, FS_DIRECTORY);

    if (new_dirent == NULL) return -EACCES;

    fs_directory_entry_release(new_dirent);

    return 0;
}

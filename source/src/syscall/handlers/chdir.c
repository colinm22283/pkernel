#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/chdir.h>

int syscall_chdir(const char * _path) {
    process_t * current_process = scheduler_current_process();

    char path[256];
    long size = process_strncpy_from_user(
        current_process,
        path,
        _path,
        256
    );
    if (size < 0) return size;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) return -ENOENT;

    if (node->type != FS_DIRECTORY) {
        fs_directory_entry_release(node);

        return -ENOTDIR;
    }

    process_set_working_dir(current_process, node);

    return 0;
}

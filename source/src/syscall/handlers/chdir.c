#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/chdir.h>

int64_t syscall_chdir(const char * _path) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, (char *) _path);
    if (path == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * node = process_open_path(current_process, path);
    if (node == NULL) return ERROR_FS_NO_ENT;

    if (node->type != FS_DIRECTORY) {
        fs_directory_entry_release(node);

        return ERROR_NOT_DIR;
    }

    process_set_working_dir(current_process, node);

    return ERROR_OK;
}

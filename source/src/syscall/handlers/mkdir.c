#include <process/scheduler.h>
#include <process/address_translation.h>

#include <syscall/handlers/mkdir.h>

#include <error_number.h>

error_number_t syscall_mkdir(const char * _path) {
    process_t * current_process = scheduler_current_process();

    const char * path = process_user_to_kernel(current_process, _path);
    if (path == NULL) return ERROR_BAD_PTR;

    fs_directory_entry_t * new_dirent = process_make_path(current_process, path, FS_DIRECTORY);
    fs_directory_entry_release(new_dirent);

    return ERROR_OK;
}

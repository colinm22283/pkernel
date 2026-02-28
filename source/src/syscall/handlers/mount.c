#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/mount.h>

#include <util/string/strcmp.h>

int syscall_mount(const char * _dst, const char * _src, const char * _fs, mount_options_t options, const char * _data) {
    process_t * current_process = scheduler_current_process();

    const char * dst = process_user_to_kernel(current_process, _dst);
    if (dst == NULL) return ERROR_BAD_PTR;

    const char * fs = process_user_to_kernel(current_process, _fs);
    if (fs == NULL) return ERROR_BAD_PTR;

    const char * data = process_user_to_kernel(current_process, _data);
    if (data == NULL) return ERROR_BAD_PTR;

    device_t * dev = NULL;
    if (_src != NULL) {
        const char * src = process_user_to_kernel(current_process, _src);
        if (src == NULL) return ERROR_BAD_PTR;

        fs_directory_entry_t * dirent = process_open_path(current_process, src);
        if (dirent == NULL) return ERROR_FS_NO_ENT;

        if (dirent->type != FS_DEVICE) return ERROR_NOT_DEV;

        dev = dirent->device;

        fs_directory_entry_release(dirent);
    }

    if (strcmp(dst, "/") == 0) {
        return fs_mount_root(fs, dev);
    }
    else {
        fs_directory_entry_t * dirent = process_open_path(current_process, dst);

        int result = fs_mount(fs, dirent, dev);

        fs_directory_entry_release(dirent);

        return result;
    }
}
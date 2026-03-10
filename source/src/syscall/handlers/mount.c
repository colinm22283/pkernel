#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/mount.h>

#include <util/string/strcmp.h>

int syscall_mount(const char * _src, const char * _dst, const char * _fs, unsigned long options, const char * _data) {
    process_t * current_process = scheduler_current_process();

    const char * dst = process_user_to_kernel(current_process, _dst);
    if (dst == NULL) return -EFAULT;

    const char * fs = process_user_to_kernel(current_process, _fs);
    if (fs == NULL) return -EFAULT;

    const char * data = process_user_to_kernel(current_process, _data);
    if (data == NULL) return -EFAULT;

    device_t * dev = NULL;
    if (_src != NULL) {
        const char * src = process_user_to_kernel(current_process, _src);
        if (src == NULL) return -EFAULT;

        fs_directory_entry_t * dirent = process_open_path(current_process, src);
        if (dirent == NULL) return -ENOENT;

        if (dirent->type != FS_DEVICE) return -ENODEV;

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

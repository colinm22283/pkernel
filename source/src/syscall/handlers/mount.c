#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <filesystem/filesystem.h>

#include <syscall/handlers/mount.h>

#include <util/string/strcmp.h>

#include <config/limits.h>

int syscall_mount(const char * _src, const char * _dst, const char * _fs, unsigned long options, const char * _data) {
    long size;

    process_t * current_process = scheduler_current_process();

    char src[LIMIT_PATH_LENGTH + 1];
    if (_src != NULL) {
        size = process_strncpy_from_user(
            current_process,
            src,
            _src,
            LIMIT_PATH_LENGTH + 1
        );
        if (size < 0) return size;
        if (size == LIMIT_PATH_LENGTH + 1) return -ENAMETOOLONG;
    }

    char dst[LIMIT_PATH_LENGTH + 1];
    size = process_strncpy_from_user(
        current_process,
        dst,
        _dst,
        LIMIT_PATH_LENGTH + 1
    );
    if (size < 0) return size;
    if (size == LIMIT_PATH_LENGTH + 1) return -ENAMETOOLONG;

    char fs[LIMIT_FS_LENGTH + 1];
    size = process_strncpy_from_user(
        current_process,
        fs,
        _fs,
        LIMIT_FS_LENGTH + 1
    );
    if (size < 0) return size;
    if (size == LIMIT_FS_LENGTH + 1) return -ENAMETOOLONG;

    device_t * dev = NULL;
    if (_src != NULL) {
        fs_directory_entry_t * dirent = process_open_path(current_process, src);
        if (dirent == NULL) return -ENOENT;

        if (dirent->type != FS_DEVICE) {
            fs_directory_entry_release(dirent);
            return -ENODEV;
        }

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

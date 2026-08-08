#include <stddef.h>

#include <syscall/handlers/seek.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

#include <debug/printf.h>

int64_t syscall_seek(fd_t fd, int64_t offset, seek_origin_t origin) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) return -EBADF;

    if (file->dirent->type == FS_DIRECTORY) return ERROR_IS_DIR;

    switch (origin) {
        case SEEK_SET: file->offset = offset; break;

        case SEEK_END: file->offset = file->dirent->node->size - offset; break;

        case SEEK_CUR: file->offset += offset; break;

        default: return ERROR_BAD_ARG;
    }

    return 0;
}

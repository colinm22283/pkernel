#include <stddef.h>

#include <syscall/handlers/seek.h>

#include <_process/address_translation.h>
#include <_process/scheduler.h>

int64_t syscall_seek(fd_t fd, int64_t offset, seek_origin_t origin) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = process_file_table_get(&current_process->file_table, fd);
    if (file == NULL) return ERROR_BAD_FD;

    if (file->dirent->type == FS_DIRECTORY) return ERROR_IS_DIR;

    switch (origin) {
        case SO_BEG: file->offset = offset; break;

        case SO_END: file->offset = file->dirent->node->size - offset; break;

        case SO_CUR: file->offset += offset; break;

        default: return ERROR_BAD_ARG;
    }

    return ERROR_OK;
}
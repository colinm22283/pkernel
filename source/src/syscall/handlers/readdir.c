#include <stddef.h>

#include <syscall/handlers/readdir.h>

#include <_process/address_translation.h>
#include <_process/scheduler.h>

int64_t syscall_readdir(fd_t fd, directory_entry_t * _entries, uint64_t size) {
    process_t * current_process = scheduler_current_process();

    directory_entry_t * entries = process_user_to_kernel(current_process, _entries);
    if (entries == NULL) return ERROR_BAD_PTR;

    fs_file_t * file = process_file_table_get(&current_process->file_table, fd);
    if (file == NULL) return ERROR_BAD_FD;

    return file_readdir(file, entries, size);
}
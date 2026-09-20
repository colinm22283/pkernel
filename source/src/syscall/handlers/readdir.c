#include <stddef.h>

#include <syscall/handlers/readdir.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>

int syscall_readdir(fd_t fd, struct dirent * _entries, size_t size) {
    process_t * current_process = scheduler_current_process();

    struct dirent * entries = heap_alloc(size);
    if (entries == NULL) return -EFAULT;

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) {
        heap_free(entries);
        return -EBADF;
    }

    int result = file_readdir(file, entries, size);
    if (result < 0) {
        heap_free(entries);
        return result;
    }

    int copy_result = process_copy_to_user(current_process, _entries, entries, result);

    heap_free(entries);
    return copy_result;
}

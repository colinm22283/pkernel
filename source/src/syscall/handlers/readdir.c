#include <stddef.h>

#include <syscall/handlers/readdir.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>

#include <debug/printf.h>

int syscall_readdir(fd_t fd, struct dirent * _entry, size_t size) {
    process_t * current_process = scheduler_current_process();

    if (size == 0) return -EFAULT;

    struct dirent * entry = heap_alloc(size);
    if (entry == NULL) return -EFAULT;

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) {
        heap_free(entry);
        return -EBADF;
    }

    int result = file_readdir(file, entry, size);
    if (result <= 0) {
        heap_free(entry);
        return result;
    }

    int copy_result = process_copy_to_user(
        current_process,
        _entry,
        entry,
        entry->d_size
    );

    if (copy_result < 0) {
        heap_free(entry);

        return copy_result;
    }

    if ((uint64_t) copy_result != entry->d_size) {
        heap_free(entry);

        return -EFAULT;
    }

    heap_free(entry);
    return result;
}

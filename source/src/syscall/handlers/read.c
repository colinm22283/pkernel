#include <stddef.h>

#include <syscall/handlers/read.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>

int64_t syscall_read(fd_t fd, char * _buffer, uint64_t size) {
    process_t * current_process = scheduler_current_process();

    char * buffer = heap_alloc(size);
    if (buffer == NULL) return -EFAULT;

    long copy_result = process_copy_from_user(
        current_process,
        buffer,
        _buffer,
        size
    );
    if (copy_result < 0) {
        heap_free(buffer);
        return -EFAULT;
    }

    fs_file_t * file = file_table_get(&current_process->file_table, fd);
    if (file == NULL) {
        heap_free(buffer);
        return -EBADF;
    }

    int64_t result = file_read(file, buffer, size);

    heap_free(buffer);

    return result;
}

#include <stddef.h>

#include <syscall/handlers/read.h>

#include <process/process.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>

int64_t syscall_read(fd_t fd, char * _buffer, uint64_t size) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = file_table_get(&current_process->file_table, fd);

    if (file == NULL) {
        return -EBADF;
    }

    char * buffer = heap_alloc(size);
    if (buffer == NULL) return -EFAULT;

    int64_t result = file_read(file, buffer, size);

    if (result < 0) {
        heap_free(buffer);
        return result;
    }

    long copy_result = process_copy_to_user(
        current_process,
        _buffer,
        buffer,
        result
    );
    if (copy_result < 0) {
        heap_free(buffer);
        return -EFAULT;
    }

    heap_free(buffer);

    return result;
}

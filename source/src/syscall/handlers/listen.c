#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/listen.h>

int syscall_listen(fd_t sock_fd, uint64_t size) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = file_table_get(&current_process->file_table, sock_fd);

    if (file == NULL) {
        return -EBADF;
    }

    if (file->dirent->type != FS_SOCKET) {
        return -ENOTSOCK;
    }

    return socket_listen(file->dirent->socket, size);
}


#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/connect.h>

int syscall_connect(fd_t sock_fd, const sockaddr_t * _sockaddr, size_t sockaddr_len) {
    process_t * current_process = scheduler_current_process();

    const sockaddr_t * sockaddr = process_user_to_kernel(current_process, _sockaddr);

    fs_file_t * file = file_table_get(&current_process->file_table, sock_fd);

    if (file == NULL) {
        return -EBADF;
    }

    if (file->dirent->type != FS_SOCKET) {
        return -ENOTSOCK;
    }

    socket_t * socket = file->dirent->socket;

    return socket_connect(socket, sockaddr, sockaddr_len);
}


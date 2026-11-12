#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <util/heap/heap.h>

#include <syscall/handlers/connect.h>

int syscall_connect(fd_t sock_fd, const sockaddr_t * _sockaddr, size_t sockaddr_len) {
    process_t * current_process = scheduler_current_process();

    sockaddr_t * sockaddr = heap_alloc(sockaddr_len);
    long copied = process_copy_from_user(current_process, sockaddr, _sockaddr, sockaddr_len);
    if (copied < 0) return copied;
    if ((size_t) copied != sockaddr_len) {
        heap_free(sockaddr);
        return -EFAULT;
    }

    fs_file_t * file = file_table_get(&current_process->file_table, sock_fd);

    if (file == NULL) {
        heap_free(sockaddr);
        return -EBADF;
    }

    if (file->dirent->type != FS_SOCKET) {
        heap_free(sockaddr);
        return -ENOTSOCK;
    }

    socket_t * socket = file->dirent->socket;

    int result = socket_connect(socket, sockaddr, sockaddr_len);

    heap_free(sockaddr);

    return result;
}


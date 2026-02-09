#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/accept.h>

fd_t syscall_accept(fd_t sock_fd) {
    process_t * current_process = scheduler_current_process();

    fs_file_t * file = file_table_get(&current_process->file_table, sock_fd);

    if (file->dirent->type != FS_SOCKET) {
        return ERROR_NOT_SOCKET;
    }

    socket_t * new_socket;

    error_number_t result = socket_accept(file->dirent->socket, &new_socket);
    if (result != ERROR_OK) return result;

    fs_directory_entry_t * dirent = fs_make_anon(FS_SOCKET);

    dirent->socket = new_socket;

    return file_table_open(&current_process->file_table, dirent, OPEN_READ | OPEN_WRITE);
}


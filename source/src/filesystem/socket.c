#include <filesystem/socket.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <util/heap/heap.h>

socket_t * socket_init(socket_domain_t domain, socket_type_t type, uint64_t protocol) {
    socket_t * socket = heap_alloc(sizeof(socket_t));

    socket->domain = domain;
    socket->type = type;
    socket->protocol = protocol;

    switch (socket->domain) {
        case SOCKET_UNIX: {
            socket->unix.path = NULL;
            socket->unix.listener = NULL;
        } break;
    }

    return socket;
}

void socket_free(socket_t * socket) {
    heap_free(socket);
}

error_number_t socket_connect(socket_t * socket, const sockaddr_t * _sockaddr, size_t sockaddr_len) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            process_t * current_process = scheduler_current_process();

            const sockaddr_unix_t * sockaddr = (const sockaddr_unix_t *) _sockaddr;

            if (sockaddr->path[sockaddr_len - 1] != '\0') return ERROR_BAD_PTR;

            fs_directory_entry_t * dirent = process_open_path(current_process, sockaddr->path);

            if (dirent == NULL) return ERROR_FS_NO_ENT;
            if (dirent->type != FS_SOCKET) return ERROR_NOT_SOCKET;

            socket->unix.path = heap_alloc(sockaddr_len);
            strcpy(socket->unix.path, sockaddr->path);

            return ERROR_OK;
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_bind(socket_t * socket, const sockaddr_t * _sockaddr, size_t sockaddr_len) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            process_t * current_process = scheduler_current_process();

            const sockaddr_unix_t * sockaddr = (const sockaddr_unix_t *) _sockaddr;

            if (sockaddr->path[sockaddr_len - 1] != '\0') return ERROR_BAD_PTR;

            {
                fs_directory_entry_t * dirent = process_open_path(current_process, sockaddr->path);

                if (dirent != NULL) {
                    fs_directory_entry_release(dirent);

                    return ERROR_EXISTS;
                }
                else {
                    fs_directory_entry_release(dirent);
                }
            }

            fs_directory_entry_t * dirent = process_make_path(current_process, sockaddr->path, FS_SOCKET);

            if (dirent == NULL) return ERROR_FS_CANT_CREATE;

            dirent->socket = socket;

            socket->unix.path = heap_alloc(sockaddr_len);
            strcpy(socket->unix.path, sockaddr->path);

            unix_socket_t * unix_socket = unix_socket_init();

            socket->unix.listener = &unix_socket->server;

            return ERROR_OK;
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_read(socket_t * socket, char * data, fs_size_t size, fs_size_t * read) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            if (socket->unix.listener == NULL) return ERROR_NOT_OPEN;

            return unix_socket_read(socket->unix.listener, data, size, read);
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_write(socket_t * socket, const char * data, fs_size_t size, fs_size_t * wrote) {


    return ERROR_UNIMPLEMENTED;
}

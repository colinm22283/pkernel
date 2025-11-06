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
            socket->unix.socket = unix_socket_init();
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

            unix_socket_t * unix_socket = dirent->socket->unix.socket;

            return unix_socket_connect(socket->unix.socket, unix_socket);
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

            socket->unix.socket = unix_socket_init();

            return ERROR_OK;
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_listen(socket_t * socket, size_t size) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            unix_socket_listen(socket->unix.socket, size);

            return ERROR_OK;
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_accept(socket_t * socket, socket_t ** _new_socket) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            unix_socket_t * unix_socket;

            error_number_t result = unix_socket_accept(socket->unix.socket, &unix_socket);
            if (result != ERROR_OK) return result;

            socket_t * new_socket = heap_alloc(sizeof(socket_t));

            new_socket->domain = socket->domain;
            new_socket->type = socket->type;
            new_socket->protocol = socket->protocol;

            new_socket->unix.socket = unix_socket;

            *_new_socket = new_socket;

            return ERROR_OK;
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_read(socket_t * socket, char * data, fs_size_t size, fs_size_t * read) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            return unix_socket_read(socket->unix.socket, data, size, read);
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

error_number_t socket_write(socket_t * socket, const char * data, fs_size_t size, fs_size_t * wrote) {
    switch (socket->domain) {
        case SOCKET_UNIX: {
            return unix_socket_write(socket->unix.socket, data, size, wrote);
        } break;
    }

    return ERROR_UNIMPLEMENTED;
}

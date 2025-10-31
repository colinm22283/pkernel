#pragma once

#include <stddef.h>
#include <stdbool.h>

#include <filesystem/types.h>

#include <socket/unix.h>

#include <error_number.h>

#include <pkos/types.h>

typedef struct {
    char * path;

    unix_listener_t * listener;
} socket_data_unix_t;

typedef struct {
    socket_domain_t domain;
    socket_type_t type;
    uint64_t protocol;

    union {
        socket_data_unix_t unix;
    };
} socket_t;

socket_t * socket_init(socket_domain_t domain, socket_type_t type, uint64_t protocol);
void socket_free(socket_t * socket);

error_number_t socket_connect(socket_t * socket, const sockaddr_t * sockaddr, size_t sockaddr_len);
error_number_t socket_bind(socket_t * socket, const sockaddr_t * sockaddr, size_t sockaddr_len);

error_number_t socket_read(socket_t * socket, char * data, fs_size_t size, fs_size_t * read);
error_number_t socket_write(socket_t * socket, const char * data, fs_size_t size, fs_size_t * wrote);

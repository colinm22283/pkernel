#pragma once

#include <scheduler/event.h>

#include <filesystem/types.h>

typedef enum {
    UNIX_STREAM,
    UNIX_DGRAM,
    UNIX_SEQPACKET = UNIX_STREAM,
} unix_socket_type_t;

typedef struct unix_packet_s {
    size_t size;
    char * data;

    struct unix_packet_s * next;
    struct unix_packet_s * prev;
} unix_packet_t;

typedef struct unix_listener_s {
    struct unix_socket_s * socket;

    unix_packet_t outgoing_head, outgoing_tail;
    unix_packet_t incoming_head, incoming_tail;

    event_t * data_ready;

    struct unix_listener_s * next;
    struct unix_listener_s * prev;
} unix_listener_t;

typedef struct unix_socket_s {
    unix_socket_type_t type;

    unix_listener_t server;

    unix_listener_t client_head, client_tail;
} unix_socket_t;

unix_socket_t * unix_socket_init(void);
void unix_socket_free(unix_socket_t * socket);

error_number_t unix_socket_read(unix_listener_t * listener, char * data, fs_size_t size, fs_size_t * read);

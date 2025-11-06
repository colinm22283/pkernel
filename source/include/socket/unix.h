#pragma once

#include <scheduler/event.h>

#include <filesystem/types.h>

struct unix_socket_s;

typedef struct unix_packet_s {
    size_t waiting_reads;

    size_t pos;
    size_t size;
    char * data;

    struct unix_packet_s * next;
    struct unix_packet_s * prev;
} unix_packet_t;

typedef struct unix_socket_listen_req_s {
    struct unix_socket_s * requester;

    struct unix_socket_listen_req_s * next;
    struct unix_socket_listen_req_s * prev;
} unix_socket_listen_req_t;

typedef struct unix_socket_s {
    event_t * listener_arrived;
    event_t * listener_accepted;
    size_t listen_queue_size, listen_queue_capacity;
    unix_socket_listen_req_t listen_queue_head, listen_queue_tail;

    event_t * data_ready;

    unix_packet_t incoming_head, incoming_tail;

    struct unix_socket_s * paired_socket;
} unix_socket_t;

unix_socket_t * unix_socket_init(void);
void unix_socket_free(unix_socket_t * socket);

void unix_socket_listen(unix_socket_t * socket, size_t listen_queue_capacity);
error_number_t unix_socket_connect(unix_socket_t * socket, unix_socket_t * target);
error_number_t unix_socket_accept(unix_socket_t * socket, unix_socket_t ** new_socket);

error_number_t unix_socket_read(unix_socket_t * socket, char * data, fs_size_t size, fs_size_t * read);
error_number_t unix_socket_write(unix_socket_t * socket, const char * data, fs_size_t size, fs_size_t * wrote);

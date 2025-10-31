#include <stddef.h>

#include <socket/unix.h>

#include <util/heap/heap.h>

unix_socket_t * unix_socket_init(void) {
    unix_socket_t * socket = heap_alloc(sizeof(unix_socket_t));

    socket->server.socket = socket;
    socket->server.data_ready = event_init();
    socket->server.next = NULL;
    socket->server.prev = NULL;

    socket->client_head.next = &socket->client_tail;
    socket->client_head.prev = NULL;
    socket->client_tail.next = NULL;
    socket->client_tail.prev = &socket->client_head;

    return socket;
}

void unix_socket_free(unix_socket_t * socket) { // TODO
    heap_free(socket);
}

error_number_t unix_socket_read(unix_listener_t * listener, char * data, fs_size_t size, fs_size_t * read) {
    listener->socket.
}

#include <stddef.h>

#include <socket/unix.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

unix_socket_t * unix_socket_init(void) {
    unix_socket_t * socket = heap_alloc_debug(sizeof(unix_socket_t), "unix_socket");

    socket->listener_arrived = event_init();
    socket->listener_accepted = event_init();

    socket->listen_queue_size = 0;
    socket->listen_queue_capacity = 0;

    socket->listen_queue_head.next = &socket->listen_queue_tail;
    socket->listen_queue_head.prev = NULL;
    socket->listen_queue_tail.next = NULL;
    socket->listen_queue_tail.prev = &socket->listen_queue_head;

    socket->data_ready = event_init();

    socket->incoming_head.next = &socket->incoming_tail;
    socket->incoming_head.prev = NULL;
    socket->incoming_tail.next = NULL;
    socket->incoming_tail.prev = &socket->incoming_head;

    socket->paired_socket = NULL;

    return socket;
}

void unix_socket_free(unix_socket_t * socket) { // TODO
    heap_free(socket);
}

void unix_socket_listen(unix_socket_t * socket, size_t listen_queue_capacity) {
    {
        unix_socket_listen_req_t * req = socket->listen_queue_head.next;
        while (req != &socket->listen_queue_tail) {
            unix_socket_listen_req_t * next = req->next;

            heap_free(req);

            req = next;
        }

        socket->listen_queue_head.next = &socket->listen_queue_tail;
        socket->listen_queue_tail.prev = &socket->listen_queue_head;
    }

    socket->listen_queue_size = 0;
    socket->listen_queue_capacity = 0;
    socket->listen_queue_capacity = listen_queue_capacity;
}

int unix_socket_connect(unix_socket_t * socket, unix_socket_t * target) {
    if (target->listen_queue_size == target->listen_queue_capacity) return ERROR_CON_REFUSED;

    unix_socket_listen_req_t * req = heap_alloc_debug(sizeof(unix_socket_listen_req_t), "unix_socket listener");

    req->requester = socket;

    req->next = &target->listen_queue_tail;
    req->prev = target->listen_queue_tail.prev;
    target->listen_queue_tail.prev->next = req;
    target->listen_queue_tail.prev = req;

    event_invoke(target->listener_arrived);
    scheduler_await(target->listener_accepted);

    return ERROR_OK;
}

int unix_socket_accept(unix_socket_t * socket, unix_socket_t ** _new_socket) {
    if (socket->listen_queue_capacity == 0) return ERROR_NOT_LISTENER;

    while (socket->listen_queue_head.next == &socket->listen_queue_tail) {
        scheduler_await(socket->listener_arrived);
    }

    unix_socket_listen_req_t * req = socket->listen_queue_head.next;

    socket->listen_queue_head.next = socket->listen_queue_head.next->next;
    socket->listen_queue_head.next->prev = &socket->listen_queue_head;

    unix_socket_t * new_socket = unix_socket_init();

    new_socket->paired_socket = req->requester;
    req->requester->paired_socket = new_socket;

    *_new_socket = new_socket;

    event_invoke(socket->listener_accepted);

    return ERROR_OK;
}

int unix_socket_read(unix_socket_t * socket, char * data, fs_size_t size, fs_size_t * read) {
    if (socket->paired_socket == NULL) {
        return ERROR_NO_ADDR;
    }

    while (socket->incoming_head.next == &socket->incoming_tail) scheduler_await(socket->data_ready);

    unix_packet_t * packet = socket->incoming_head.next;

    if (packet->size - packet->pos > size) {
        memcpy(data, packet->data, size);

        packet->pos += size;

        *read = size;
    }
    else if (packet->size - packet->pos < size) {
        packet->next->prev = packet->prev;
        packet->prev->next = packet->next;

        memcpy(data, packet->data, packet->size - packet->pos);

        *read = packet->size - packet->pos;

        heap_free(packet->data);
        heap_free(packet);
    }
    else {
        packet->next->prev = packet->prev;
        packet->prev->next = packet->next;

        memcpy(data, packet->data, size);

        *read = size;

        heap_free(packet->data);
        heap_free(packet);
    }

    return ERROR_OK;
}

int unix_socket_write(unix_socket_t * socket, const char * data, fs_size_t size, fs_size_t * wrote) {
    if (socket->paired_socket == NULL) {
        return ERROR_NO_ADDR;
    }

    unix_packet_t * packet = heap_alloc_debug(sizeof(unix_packet_t), "unix_socket packet");

    packet->pos = 0;
    packet->size = size;
    packet->data = heap_alloc_debug(size, "unix_socket packet data");
    memcpy(packet->data, data, size);

    packet->next = &socket->paired_socket->incoming_tail;
    packet->prev = socket->paired_socket->incoming_tail.prev;

    socket->paired_socket->incoming_tail.prev->next = packet;
    socket->paired_socket->incoming_tail.prev = packet;

    event_invoke(socket->paired_socket->data_ready);

    *wrote = size;

    return ERROR_OK;
}

#include <stddef.h>

#include <io/arbitrator.h>

#include <util/heap/heap.h>

#define IO_ARBITRATOR_BUCKET_COUNT (0x100)

#define IO_ARBITRATOR_HASH(port) (port % IO_ARBITRATOR_BUCKET_COUNT)

typedef struct io_arbitrator_node_s {
    port_t port;

    struct io_arbitrator_node_s * next;
} io_arbitrator_node_t;

typedef struct {
    io_arbitrator_node_t * head;
} io_arbitrator_list_t;

io_arbitrator_list_t io_arbitrator_buckets[IO_ARBITRATOR_BUCKET_COUNT];

void io_arbitrator_init(void) {
    for (uint64_t i = 0; i < IO_ARBITRATOR_BUCKET_COUNT; i++) {
        io_arbitrator_buckets[i].head = NULL;
    }
}

bool io_arbitrator_test_reserved(port_t port) {
    uint64_t hash = IO_ARBITRATOR_HASH(port);

    io_arbitrator_node_t * node = io_arbitrator_buckets[hash].head;

    while (node != NULL) {
        if (node->port == port) return true;

        node = node->next;
    }

    return false;
}

bool io_arbitrator_reserve(port_t port) {
    if (io_arbitrator_test_reserved(port)) return false;

    io_arbitrator_node_t * new_node = heap_alloc(sizeof(io_arbitrator_node_t));
    new_node->port = port;

    uint64_t hash = IO_ARBITRATOR_HASH(port);

    new_node->next = io_arbitrator_buckets[hash].head;
    io_arbitrator_buckets[hash].head = new_node;

    return true;
}

bool io_arbitrator_release(port_t port) {
    for (uint64_t i = 0; i < IO_ARBITRATOR_BUCKET_COUNT; i++) {
        io_arbitrator_node_t ** node = &io_arbitrator_buckets[i].head;

        while (*node != NULL) {
            if ((*node)->port == port) {
                io_arbitrator_node_t * old_node = *node;

                *node = (*node)->next;

                heap_free(old_node);

                return true;
            }

            node = &(*node)->next;
        }
    }

    return false;
}
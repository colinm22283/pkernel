#include <stddef.h>

#include <filesystem/node_number_allocator.h>

#include <util/heap/heap.h>

node_number_allocator_node_t node_number_allocator_head, node_number_allocator_tail;

bool node_number_allocator_init(void) {
    node_number_allocator_node_t * new_node = heap_alloc(sizeof(node_number_allocator_node_t));
    new_node->next = &node_number_allocator_tail;
    new_node->prev = &node_number_allocator_head;
    new_node->reserved = false;
    new_node->start = 1;
    new_node->region_size = FILESYSTEM_NODE_NUMBER_MAX - new_node->start;

    node_number_allocator_head.next = new_node;
    node_number_allocator_head.prev = NULL;
    node_number_allocator_tail.next = NULL;
    node_number_allocator_tail.prev = new_node;

    return true;
}

node_number_allocator_node_t * node_number_allocator_alloc(uint64_t node_count) {
    node_number_allocator_node_t * node = node_number_allocator_head.next;

    while (node != &node_number_allocator_tail) {
        if (!node->reserved) {
            if (node->region_size <= node_count) {
                if (node->region_size == node_count) {
                    node->reserved = true;

                    return node;
                }
                else {
                    node_number_allocator_node_t * new_node = heap_alloc(sizeof(node_number_allocator_node_t));

                    new_node->reserved = true;
                    new_node->region_size = node_count;
                    new_node->start = node->start;

                    node->start += node_count;
                    node->region_size -= node_count;

                    new_node->next = node;
                    new_node->prev = node->prev;
                    node->prev->next = new_node;
                    node->prev = new_node;

                    return new_node;
                }
            }
        }

        node = node->next;
    }

    return false;
}

bool node_number_allocator_free(node_number_allocator_node_t * node) {
    if (!node->reserved) return false;

    if (node->next != &node_number_allocator_tail && !node->next->reserved) {
        node_number_allocator_node_t * next_node = node->next;

        node->region_size += node->next->region_size;

        node->next = node->next->next;

        heap_free(next_node);
    }

    if (node->prev != &node_number_allocator_head && !node->prev->reserved) {
        node_number_allocator_node_t * old_node = node->prev;

        node = node->prev;

        node->next = old_node->next;
        node->region_size += old_node->region_size;

        heap_free(old_node);
    }

    node->reserved = false;

    return true;
}

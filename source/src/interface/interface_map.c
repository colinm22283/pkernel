#include <stddef.h>

#include <interface/interface_map.h>

#include <util/heap/heap.h>

#include <util/string/strcmp.h>

interface_map_t interface_map;

void interface_map_init(void) {
    interface_map.head.next = &interface_map.tail;
    interface_map.head.prev = NULL;

    interface_map.tail.next = NULL;
    interface_map.tail.prev = &interface_map.head;
}

void interface_map_free(void) {
    interface_map_node_t * node = interface_map.head.next;

    while (node != &interface_map.tail) {
        interface_map_node_t * next = node->next;

        heap_free(node);

        node = next;
    }
}

interface_map_node_t * interface_map_add(const char * name) {
    uint64_t instance_number = 0;
    interface_map_node_t * node = interface_map.head.next;
    while (node != &interface_map.tail) {
        if (strcmp(node->interface.name, name) == 0) instance_number++;

        node = node->next;
    }

    interface_map_node_t * new_node = heap_alloc(sizeof(interface_map_node_t));

    new_node->instance_number = instance_number;
    interface_init(&new_node->interface, name, true);

    new_node->next = interface_map.head.next;
    new_node->prev = &interface_map.head;

    interface_map.head.next->prev = new_node;
    interface_map.head.next = new_node;

    return new_node;
}

interface_map_node_t * interface_map_add_unprotected(const char * name) {
    interface_map_node_t * new_node = heap_alloc(sizeof(interface_map_node_t));

    interface_init(&new_node->interface, name, false);

    new_node->next = interface_map.head.next;
    new_node->prev = &interface_map.head;

    interface_map.head.next->prev = new_node;
    interface_map.head.next = new_node;

    return new_node;
}

void interface_map_remove(interface_map_node_t * node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;

    interface_free(&node->interface);

    heap_free(node);
}

interface_t * interface_map_lookup(const char * name) {
    interface_map_node_t * node = interface_map.head.next;

    while (node != &interface_map.tail) {
        interface_map_node_t * next = node->next;

        if (strcmp(node->interface.name, name) == 0) return &node->interface;

        node = next;
    }

    return NULL;
}

interface_t * interface_map_lookup_instance(const char * name, uint64_t instance_number) {
    interface_map_node_t * node = interface_map.head.next;

    while (node != &interface_map.tail) {
        interface_map_node_t * next = node->next;

        if (strcmp(node->interface.name, name) == 0 && node->instance_number == instance_number) return &node->interface;

        node = next;
    }

    return NULL;
}
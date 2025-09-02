#include <stddef.h>

#include <filesystem/mapping_registry.h>

#include <util/heap/heap.h>

bool filesystem_mapping_registry_init(filesystem_mapping_registry_t * registry) {
    registry->head.next = &registry->tail;
    registry->head.prev = NULL;
    registry->tail.next = NULL;
    registry->tail.prev = &registry->head;

    return true;
}

bool filesystem_mapping_registry_free(filesystem_mapping_registry_t * registry) {
    return false; // TODO
}

void ** filesystem_mapping_registry_add(filesystem_mapping_registry_t * registry, pml4t64_t * pml4t, void * vaddr) {
    filesystem_mapping_registry_node_t * new_node = heap_alloc(sizeof(filesystem_mapping_registry_node_t));

    new_node->pml4t = pml4t;
    new_node->vaddr = vaddr;

    new_node->next = registry->head.next;
    new_node->prev = &registry->head;

    registry->head.next->prev = new_node;
    registry->head.next = new_node;

    return &new_node->private;
}

void filesystem_mapping_registry_remove(filesystem_mapping_registry_t * registry, pml4t64_t * pml4t, void * vaddr); // TODO
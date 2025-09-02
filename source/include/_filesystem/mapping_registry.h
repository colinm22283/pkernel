#pragma once

#include <stdbool.h>

#include <sys/paging/pml4t.h>

typedef struct filesystem_mapping_registry_node_s {
    pml4t64_t * pml4t;
    void * vaddr;
    void * private;

    struct filesystem_mapping_registry_node_s * next;
    struct filesystem_mapping_registry_node_s * prev;
} filesystem_mapping_registry_node_t;

typedef struct {
    filesystem_mapping_registry_node_t head, tail;
} filesystem_mapping_registry_t;

bool filesystem_mapping_registry_init(filesystem_mapping_registry_t * registry);
bool filesystem_mapping_registry_free(filesystem_mapping_registry_t * registry);

void ** filesystem_mapping_registry_add(filesystem_mapping_registry_t * registry, pml4t64_t * pml4t, void * vaddr);
void filesystem_mapping_registry_remove(filesystem_mapping_registry_t * registry, pml4t64_t * pml4t, void * vaddr);

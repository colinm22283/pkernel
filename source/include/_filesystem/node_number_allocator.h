#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <filesystem/node.h>

typedef struct node_number_allocator_node_s {
    bool reserved;
    filesystem_node_number_t start;
    uint64_t region_size;

    struct node_number_allocator_node_s * next;
    struct node_number_allocator_node_s * prev;
} node_number_allocator_node_t;

bool node_number_allocator_init(void);

node_number_allocator_node_t * node_number_allocator_alloc(uint64_t node_count);
bool node_number_allocator_free(node_number_allocator_node_t * alloc);

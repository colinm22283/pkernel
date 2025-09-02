#pragma once

#include <stdbool.h>

struct superblock_s;

typedef struct filesystem_node_s * (* superblock_create_node_t)(struct superblock_s * superblock);
typedef bool (* superblock_delete_node_t)(struct superblock_s * superblock, struct filesystem_node_s * node);
typedef struct filesystem_node_s * (* superblock_unmount_t)(struct superblock_s * superblock);

typedef struct superblock_operations_s {
    superblock_create_node_t create_node;
    superblock_delete_node_t delete_node;
    superblock_unmount_t unmount;
} superblock_operations_t;

typedef struct superblock_s {
    superblock_operations_t operations;

    void * private;
} superblock_t;
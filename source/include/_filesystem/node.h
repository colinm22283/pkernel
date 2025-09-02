#pragma once

#include <stdint.h>

#include <filesystem/superblock.h>
#include <filesystem/directory_entries.h>
#include <filesystem/mapping_registry.h>

#define FILESYSTEM_NULL_NODE_NUMBER ((filesystem_node_number_t) 0)

typedef uint64_t filesystem_node_number_t;
#define FILESYSTEM_NODE_NUMBER_MAX ((filesystem_node_number_t) UINT64_MAX)

typedef enum {
    NT_DIRECTORY,
    NT_REGULAR,
    NT_DEVICE,
} filesystem_node_type_t;

typedef struct {
    uint64_t (* write)(struct filesystem_node_s * node, const char * buffer, uint64_t size, uint64_t offset);
    uint64_t (* read)(struct filesystem_node_s * node, char * buffer, uint64_t size, uint64_t offset);
    bool (* map)(struct filesystem_node_s * node, void ** private, pml4t64_t * pml4t, void * map_addr, uint64_t size, uint64_t offset);
    bool (* unmap)(struct filesystem_node_s * node, void * private);
    bool (* link)(struct filesystem_node_s * node, struct filesystem_node_s * subnode, const char * name);


} filesystem_node_operations_t;

typedef struct filesystem_node_s {
    filesystem_node_type_t type;
    filesystem_node_number_t node_number;

    struct superblock_s * superblock;

    filesystem_node_operations_t * operations;

    uint64_t size;

    filesystem_mapping_registry_t mapping_registry;

    void * private;
    directory_entries_t * directory_entries;
} filesystem_node_t;

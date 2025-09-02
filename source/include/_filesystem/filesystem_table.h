#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

struct device_s;

typedef bool (* filesystem_table_mount_t)(superblock_t * superblock, filesystem_node_t * mount_point, struct device_s * device_path);

typedef struct filesystem_table_entry_s {
    char * name;

    filesystem_table_mount_t mount;
    superblock_operations_t superblock_operations;

    struct filesystem_table_entry_s * next;
    struct filesystem_table_entry_s * prev;
} filesystem_table_entry_t;

bool filesystem_table_init(void);

filesystem_table_entry_t * filesystem_table_push(const char * name, filesystem_table_mount_t mount, superblock_operations_t * superblock_operations);
bool filesystem_table_remove(filesystem_table_entry_t * entry);

filesystem_table_entry_t * filesystem_table_lookup(const char * name);
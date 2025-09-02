#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

typedef struct {
    uint64_t size;
    char * data;
} regular_file_data_t;

struct device_s;

extern filesystem_node_operations_t ramfs_node_operations;

bool ramfs_init(void);

bool ramfs_mount(superblock_t * superblock, filesystem_node_t * mount_point, struct device_s * device);

filesystem_node_t * ramfs_create_node(superblock_t * superblock);
bool ramfs_delete_node(superblock_t * superblock, struct filesystem_node_s * node);
filesystem_node_t * ramfs_unmount(superblock_t * superblock);

uint64_t ramfs_write(filesystem_node_t * node, const char * buffer, uint64_t size, uint64_t offset);
uint64_t ramfs_read(filesystem_node_t * node, char * buffer, uint64_t size, uint64_t offset);
bool ramfs_link(filesystem_node_t * node, filesystem_node_t * subnode, const char * name);

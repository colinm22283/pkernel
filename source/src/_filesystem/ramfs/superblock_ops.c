#include <stddef.h>

#include <filesystem/ramfs/ramfs.h>

#include <util/heap/heap.h>

bool ramfs_mount(superblock_t * superblock, filesystem_node_t * mount_point, struct device_s * device) {
    mount_point->operations = &ramfs_node_operations;

    return true;
}

filesystem_node_t * ramfs_create_node(superblock_t * superblock) {
    filesystem_node_t * new_node = heap_alloc(sizeof(filesystem_node_t));

    new_node->superblock = superblock;
//    TODO: new_node->node_number
    new_node->operations = &ramfs_node_operations;
    new_node->private = NULL;

    return new_node;
}

bool ramfs_delete_node(superblock_t * superblock, struct filesystem_node_s * node) {
    heap_free(node);

    return true;
}

filesystem_node_t * ramfs_unmount(superblock_t * superblock) {
    // TODO
}
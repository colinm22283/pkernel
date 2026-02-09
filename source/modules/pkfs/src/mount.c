#include <stddef.h>

#include <util/heap/heap.h>

#include <pkfs.h>
#include <disc_operations.h>

error_number_t mount(fs_superblock_t * superblock) {
    superblock->superblock_ops = &superblock_ops;

    fs_directory_entry_t * mount_point = superblock->mount_point;

    fs_node_t * _root_node = heap_alloc_debug(sizeof(pkfs_fs_node_t), "pkfs root node");
    pkfs_fs_node_t * root_node = (pkfs_fs_node_t *) _root_node;

    fs_node_init(&root_node->base);

    mount_point->node = _root_node;

    root_node->file_page = open_filesystem(superblock->device, 64);

    return ERROR_OK;
}

error_number_t unmount(fs_superblock_t * superblock) {
    return ERROR_UNIMPLEMENTED;
}

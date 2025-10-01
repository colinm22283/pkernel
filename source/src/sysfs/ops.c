#include <stddef.h>

#include <sysfs/internal.h>

#include <filesystem/directory_entry.h>
#include <filesystem/superblock.h>

#include <util/heap/heap.h>

error_number_t sysfs_read(fs_directory_entry_t * dirent, char * data, fs_size_t size, fs_size_t offset, fs_size_t * read) {
    sysfs_fs_node_t * node = (sysfs_fs_node_t *) dirent->node;

    int64_t result = node->read(node->id, data, size, offset);

    if (result < 0) {
        *read = 0;

        return result;
    }
    else {
        *read = result;

        return ERROR_OK;
    }
}

error_number_t sysfs_write(fs_directory_entry_t * dirent, const char * data, fs_size_t size, fs_size_t offset, fs_size_t * wrote) {
    sysfs_fs_node_t * node = (sysfs_fs_node_t *) dirent->node;

    int64_t result = node->write(node->id, data, size, offset);

    if (result < 0) {
        *wrote = 0;

        return result;
    }
    else {
        *wrote = result;

        return ERROR_OK;
    }
}

fs_node_t * sysfs_alloc_node(fs_superblock_t * superblock) {
    sysfs_fs_node_t * new_node = heap_alloc(sizeof(sysfs_fs_node_t));

    fs_node_init(&new_node->base);

    new_node->id = 0;
    new_node->read = NULL;
    new_node->write = NULL;

    return (fs_node_t *) new_node;
}

error_number_t sysfs_free_node(fs_superblock_t * superblock, fs_node_t * node) {
    heap_free(node);

    return ERROR_OK;
}

error_number_t sysfs_list(fs_directory_entry_t * dirent) {
    fs_directory_entry_add_reference(dirent);

    return ERROR_OK;
}

fs_directory_entry_node_t * sysfs_create(struct fs_directory_entry_s * parent, struct fs_node_s * _node, const char * name, fs_file_type_t type) {
    fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(parent, name);

    fs_directory_entry_t * new_dirent = fs_directory_entry_create(type, parent, dirent_node);
    fs_directory_entry_add_reference(new_dirent);

    new_dirent->node = _node;

    dirent_node->dirent = new_dirent;

    return dirent_node;
}

const fs_superblock_ops_t sysfs_superblock_ops = {
    .alloc_node = sysfs_alloc_node,
    .free_node  = sysfs_free_node,

    .list = sysfs_list,
    .create = sysfs_create,

    .read = sysfs_read,
    .write = sysfs_write,
};
#include <stddef.h>

#include <filesystem/filesystem.h>
#include <filesystem/ramfs/ramfs.h>

#include <device/device.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

#include <debug/vga_print.h>

#include "sys/halt.h"

typedef struct {
    fs_node_t base;

    uint64_t size;
    char * data;
} ramfs_fs_node_t;

fs_node_t * ramfs_alloc_node(fs_superblock_t * superblock) {
    // vga_print("RAMFS CREATE NODE\n");

    ramfs_fs_node_t * new_node = heap_alloc_debug(sizeof(ramfs_fs_node_t), "ramfs fsnode");

    fs_node_init(&new_node->base);
    new_node->size = 0;
    new_node->data = NULL;

    return (fs_node_t *) new_node;
}

error_number_t ramfs_free_node(fs_superblock_t * superblock, fs_node_t * node) {
    // vga_print("RAMFS FREE NODE\n");

    heap_free(node);

    return ERROR_OK;
}

error_number_t ramfs_list(fs_directory_entry_t * dirent) {
    fs_directory_entry_add_reference(dirent);

    return ERROR_OK;
}

fs_directory_entry_node_t * ramfs_create(struct fs_directory_entry_s * parent, struct fs_node_s * _node, const char * name, fs_file_type_t type) {
    ramfs_fs_node_t * node = (ramfs_fs_node_t *) _node;

    switch (type) {
        case FS_REGULAR: {
            node->size = 0;
            node->data = heap_alloc_debug(1, "ramfs data");
        } break;

        case FS_DIRECTORY: case FS_PIPE: case FS_SOCKET: break;

        default: return NULL;
    }

    fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(parent, name);

    fs_directory_entry_t * new_dirent = fs_directory_entry_create(type, parent, dirent_node);
    fs_directory_entry_add_reference(new_dirent);

    new_dirent->node = _node;

    dirent_node->dirent = new_dirent;

    return dirent_node;
}

fs_directory_entry_node_t * ramfs_link(fs_directory_entry_t * dirent, fs_directory_entry_t * subdirent, const char * name) {
    fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(dirent, name);

    fs_directory_entry_t * new_dirent = fs_directory_entry_create(subdirent->type, dirent, dirent_node);

    dirent_node->dirent = new_dirent;

    return dirent_node;
}

error_number_t ramfs_read(fs_directory_entry_t * dirent, char * data, fs_size_t size, fs_size_t offset, fs_size_t * read) {
    ramfs_fs_node_t * node = (ramfs_fs_node_t *) dirent->node;

    if (offset >= node->size) {
        *read = 0;
        return ERROR_OK;
    }

    uint64_t limited_size;
    if (size + offset > node->size) limited_size = node->size - offset;
    else limited_size = size;

    memcpy(data, node->data + offset, limited_size);
    *read = limited_size;

    return ERROR_OK;
}

error_number_t ramfs_write(fs_directory_entry_t * dirent, const char * data, fs_size_t size, fs_size_t offset, fs_size_t * wrote) {
    // vga_print("writing!\n");
    ramfs_fs_node_t * node = (ramfs_fs_node_t *) dirent->node;

    // vga_print("buffer: ");
    // vga_print_hex((uint64_t) node->data);
    // vga_print("\n");

    if (offset + size > node->size) {
        node->size = offset + size;

        node->data = heap_realloc(node->data, node->size);
    }

    memcpy(node->data + offset, data, size);

    return ERROR_OK;
}

const fs_superblock_ops_t ramfs_superblock_ops = {
    .alloc_node = ramfs_alloc_node,
    .free_node   = ramfs_free_node,

    .list = ramfs_list,

    .create = ramfs_create,

    .link = ramfs_link,

    .read = ramfs_read,
    .write = ramfs_write,
};

error_number_t fs_ramfs_mount(fs_superblock_t * superblock) {
    return ERROR_OK;
}

error_number_t fs_ramfs_unmount(__MAYBE_UNUSED fs_superblock_t * superblock) {
    return ERROR_OK;
}

bool fs_ramfs_init(void) {
    fs_register("ramfs", &ramfs_superblock_ops, fs_ramfs_mount, fs_ramfs_unmount);

    return true;
}

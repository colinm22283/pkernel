#include <stddef.h>

#include <device/devfs.h>

#include <filesystem/filesystem.h>

#include <util/heap/heap.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#include <debug/vga_print.h>

#include "sys/halt.h"

typedef struct {
    fs_node_t base;

    device_t * device;
} devfs_fs_node_t;

devfs_entry_t devfs_head, devfs_tail;

uint64_t devfs_mount_count;
fs_directory_entry_t ** devfs_mounts;

error_number_t devfs_free(fs_superblock_t * superblock, fs_node_t * node) {
    heap_free(node);

    return ERROR_OK;
}

error_number_t devfs_list(fs_directory_entry_t * dirent) {
    for (
        devfs_entry_t * entry = devfs_head.next;
        entry != &devfs_tail;
        entry = entry->next
    ) {
        devfs_fs_node_t * new_node = heap_alloc(sizeof(devfs_fs_node_t));
        fs_node_init(&new_node->base);

        new_node->device = entry->device;

        fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(dirent, entry->name);

        fs_directory_entry_t * new_dirent = fs_directory_entry_create(FS_DEVICE, dirent, dirent_node);

        dirent_node->dirent = new_dirent;
    }

    return ERROR_OK;
}

error_number_t devfs_write(fs_directory_entry_t * dirent, const char * data, uint64_t size, uint64_t offset, uint64_t * wrote) {
    devfs_fs_node_t * node = (devfs_fs_node_t *) dirent->node;
    device_t * device = node->device;

    *wrote = device_write(device, data, size, offset);

    return ERROR_OK;
}

error_number_t devfs_read(fs_directory_entry_t * dirent, char * data, uint64_t size, uint64_t offset, uint64_t * read) {
    devfs_fs_node_t * node = (devfs_fs_node_t *) dirent->node;
    device_t * device = node->device;

    *read = device_read(device, data, size, offset);

    return ERROR_OK;
}

fs_superblock_ops_t devfs_superblock_ops = {
    .free_node = devfs_free,

    .list  = devfs_list,

    .write = devfs_write,
    .read  = devfs_read,
};

error_number_t devfs_mount(fs_superblock_t * superblock) {
    superblock->superblock_ops = &devfs_superblock_ops;

    fs_directory_entry_t * mount_point = superblock->mount_point;

    fs_node_t * _root_node = heap_alloc(sizeof(devfs_fs_node_t));
    devfs_fs_node_t * root_node = (devfs_fs_node_t *) _root_node;

    fs_node_init(&root_node->base);

    mount_point->node = _root_node;

    devfs_mounts[devfs_mount_count++] = superblock->mount_point;
    devfs_mounts = heap_realloc(devfs_mounts, (devfs_mount_count + 1) * sizeof(fs_directory_entry_t *));

    return ERROR_OK;
}

error_number_t devfs_unmount(__MAYBE_UNUSED fs_superblock_t * superblock) {
    // TODO

    return ERROR_OK;
}

error_number_t devfs_init(void) {
    devfs_head.next = &devfs_tail;
    devfs_head.prev = NULL;
    devfs_tail.next = NULL;
    devfs_tail.prev = &devfs_head;

    error_number_t result;

    result = fs_register("devfs", devfs_mount, devfs_unmount);
    if (result != ERROR_OK) return result;

    devfs_mount_count = 0;
    devfs_mounts = heap_alloc(sizeof(fs_directory_entry_t *));

    fs_directory_entry_t * dev_node = fs_directory_entry_enter(&fs_root, "dev");

    result = fs_mount("devfs", dev_node, NULL);
    if (result != ERROR_OK) return result;

    // fs_directory_entry_release(root_dirent);

    return ERROR_OK;
}

devfs_entry_t * devfs_register(device_t * device) {
    devfs_entry_t * new_entry = heap_alloc(sizeof(devfs_entry_t));

    new_entry->name = heap_alloc(strlen(device->name) + 1);
    strcpy(new_entry->name, device->name);

    new_entry->device = device;

    new_entry->next = devfs_head.next;
    new_entry->prev = &devfs_head;
    devfs_head.next->prev = new_entry;
    devfs_head.next = new_entry;

    devfs_fs_node_t * new_node = heap_alloc(sizeof(devfs_fs_node_t));
    fs_node_init(&new_node->base);

    new_node->device = device;

    for (uint64_t i = 0; i < devfs_mount_count; i++) {
        new_node->device = device;

        fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(devfs_mounts[i], device->name);

        fs_directory_entry_t * new_dirent = fs_directory_entry_create(FS_DEVICE, devfs_mounts[i], dirent_node);
        new_dirent->node = (fs_node_t *) new_node;

        dirent_node->dirent = new_dirent;
    }

    return new_entry;
}

error_number_t devfs_remove(devfs_entry_t * entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    heap_free(entry->name);
    heap_free(entry);

    return ERROR_OK;
}

device_t * devfs_open(fs_node_t * devfs_node) {
    return ((devfs_fs_node_t *) devfs_node)->device;
}

#include <stddef.h>

#include <devfs/devfs.h>

#include <filesystem/filesystem.h>

#include <util/heap/heap.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#include <mod_defs.h>

devfs_entry_t devfs_head, devfs_tail;

uint64_t devfs_mount_count;
fs_directory_entry_t ** devfs_mounts;

__MOD_EXPORT fs_node_t * devfs_alloc(fs_superblock_t * superblock) {
    return heap_alloc_debug(sizeof(fs_node_t), "devfs alloc fsnode");
}

__MOD_EXPORT error_number_t devfs_free(fs_superblock_t * superblock, fs_node_t * node) {
    heap_free(node);

    return ERROR_OK;
}

__MOD_EXPORT error_number_t devfs_list(fs_directory_entry_t * dirent) {
    for (
        devfs_entry_t * entry = devfs_head.next;
        entry != &devfs_tail;
        entry = entry->next
    ) {
        fs_node_t * new_node = heap_alloc_debug(sizeof(fs_node_t), "devfs list fsnode");
        fs_node_init(new_node);

        fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(dirent, entry->name);

        fs_directory_entry_t * new_dirent = fs_directory_entry_create(FS_DEVICE, dirent, dirent_node);

        dirent_node->dirent = new_dirent;
        new_dirent->node = (fs_node_t *) new_node;
        new_dirent->device = entry->device;
    }

    return ERROR_OK;
}

__MOD_EXPORT error_number_t devfs_write(fs_directory_entry_t * dirent, const char * data, uint64_t size, uint64_t offset, uint64_t * wrote) {
    device_t * device = dirent->device;

    *wrote = device_write(device, data, size, offset);

    return ERROR_OK;
}

__MOD_EXPORT error_number_t devfs_read(fs_directory_entry_t * dirent, char * data, uint64_t size, uint64_t offset, uint64_t * read) {
    device_t * device = dirent->device;

    *read = device_read(device, data, size, offset);

    return ERROR_OK;
}

fs_superblock_ops_t devfs_superblock_ops = {
    .alloc_node = devfs_alloc,
    .free_node = devfs_free,

    .list  = devfs_list,

    .write = devfs_write,
    .read  = devfs_read,
};

__MOD_EXPORT error_number_t devfs_mount(fs_superblock_t * superblock) {
    devfs_mounts[devfs_mount_count++] = superblock->mount_point;
    devfs_mounts = heap_realloc(devfs_mounts, (devfs_mount_count + 1) * sizeof(fs_directory_entry_t *));

    return ERROR_OK;
}

__MOD_EXPORT error_number_t devfs_unmount(fs_superblock_t * superblock) {
    for (uint64_t i = 0; i < devfs_mount_count; i++) {
        if (devfs_mounts[i] == superblock->mount_point) {
            for (i++; i < devfs_mount_count; i++) {
                devfs_mounts[i - 1] = devfs_mounts[i];
            }

            devfs_mount_count--;

            return ERROR_OK;
        }
    }

    return ERROR_UNKNOWN;
}

__MOD_EXPORT devfs_entry_t * devfs_register(device_t * device) {
    devfs_entry_t * new_entry = heap_alloc_debug(sizeof(devfs_entry_t), "devfs entry");

    new_entry->name = heap_alloc_debug(strlen(device->name) + 1, "devfs entry name");
    strcpy(new_entry->name, device->name);

    new_entry->device = device;

    new_entry->next = devfs_head.next;
    new_entry->prev = &devfs_head;
    devfs_head.next->prev = new_entry;
    devfs_head.next = new_entry;

    fs_node_t * new_node = heap_alloc_debug(sizeof(fs_node_t), "devfs register fsnode");
    fs_node_init(new_node);

    for (uint64_t i = 0; i < devfs_mount_count; i++) {
        fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(devfs_mounts[i], device->name);

        fs_directory_entry_t * new_dirent = fs_directory_entry_create(FS_DEVICE, devfs_mounts[i], dirent_node);
        new_dirent->node = new_node;
        new_dirent->device = device;

        new_node->references++;

        dirent_node->dirent = new_dirent;
    }

    return new_entry;
}

__MOD_EXPORT error_number_t devfs_remove(devfs_entry_t * entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    heap_free(entry->name);
    heap_free(entry);

    return ERROR_OK;
}

error_number_t init(void) {
    devfs_head.next = &devfs_tail;
    devfs_head.prev = NULL;
    devfs_tail.next = NULL;
    devfs_tail.prev = &devfs_head;

    fs_register("devfs", &devfs_superblock_ops, devfs_mount, devfs_unmount);

    devfs_mount_count = 0;
    devfs_mounts = heap_alloc_debug(sizeof(fs_directory_entry_t *), "devfs mounts");

    return ERROR_OK;
}

error_number_t free(void) { return ERROR_OK; }

MODULE_NAME("devfs");
MODULE_DEPS_NONE();

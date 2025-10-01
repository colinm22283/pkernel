#include <sysfs/internal.h>

#include <filesystem/filesystem.h>

#include <util/heap/heap.h>

#include <debug/vga_print.h>

error_number_t sysfs_mount(fs_superblock_t * superblock) {
    sysfs_mounts[sysfs_mount_count++].mount_point = superblock->mount_point;
    sysfs_mounts = heap_realloc(sysfs_mounts, (sysfs_mount_count + 1) * sizeof(sysfs_mount_t));

    for (sysfs_entry_t * entry = sysfs_head.next; entry != &sysfs_tail; entry = entry->next) {
        fs_directory_entry_t * dirent = fs_make_path_dirs(superblock->mount_point, entry->path, FS_REGULAR);

        sysfs_fs_node_t * node = (sysfs_fs_node_t *) dirent->node;

        node->id = entry->id;
        node->read = entry->read;
        node->write = entry->write;
    }

    return ERROR_OK;
}

error_number_t sysfs_unmount(fs_superblock_t * superblock) {
    return ERROR_UNIMPLEMENTED;
}

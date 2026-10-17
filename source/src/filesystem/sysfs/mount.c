#include <filesystem/sysfs/internal.h>

#include <filesystem/filesystem.h>

#include <util/heap/heap.h>

int fs_sysfs_mount(fs_superblock_t * superblock) {
    sysfs_mounts[sysfs_mount_count++].mount_point = superblock->mount_point;
    sysfs_mounts = heap_realloc(sysfs_mounts, (sysfs_mount_count + 1) * sizeof(fs_sysfs_mount_t));

    for (fs_sysfs_entry_t * entry = sysfs_head.next; entry != &sysfs_tail; entry = entry->next) {
        fs_directory_entry_t * dirent = fs_make_path_dirs(superblock->mount_point, entry->path, FS_REGULAR);

        fs_sysfs_fs_node_t * node = (fs_sysfs_fs_node_t *) dirent->node;

        node->id = entry->id;
        node->read = entry->read;
        node->write = entry->write;
    }

    return 0;
}

void fs_sysfs_unmount_recur(fs_directory_entry_t * dirent) {
    fs_directory_entry_node_t * node = dirent->head.next;

    while (node != &dirent->tail) {
        fs_directory_entry_node_t * next = node->next;

        if (node->dirent != NULL) {
            fs_sysfs_unmount_recur(node->dirent);

            fs_directory_entry_release(node->dirent);
        }

        node = next;
    }
}

int fs_sysfs_unmount(fs_superblock_t * superblock) {
    for (uint64_t i = 0; i < sysfs_mount_count; i++) {
        if (sysfs_mounts[i].mount_point == superblock->mount_point) {
            sysfs_mount_count--;
            for (uint64_t j = i; j < sysfs_mount_count; j++) {
                sysfs_mounts[j] = sysfs_mounts[j + 1];
            }

            sysfs_mounts = heap_realloc(sysfs_mounts, (sysfs_mount_count + 1) * sizeof(fs_sysfs_mount_t));

            break;
        }
    }

    fs_sysfs_unmount_recur(superblock->mount_point);

    return 0;
}


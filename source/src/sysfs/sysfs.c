#include <stddef.h>

#include <sysfs/sysfs.h>
#include <sysfs/internal.h>

#include <filesystem/node.h>
#include <filesystem/file.h>

#include <util/heap/heap.h>

#include <util/string/strcmp.h>

sysfs_entry_t sysfs_head, sysfs_tail;

uint64_t sysfs_mount_count;
sysfs_mount_t * sysfs_mounts;

error_number_t sysfs_init(void) {
    sysfs_head.next = &sysfs_tail;
    sysfs_head.prev = NULL;
    sysfs_tail.next = NULL;
    sysfs_tail.prev = &sysfs_head;

    sysfs_mount_count = 0;
    sysfs_mounts = heap_alloc(sizeof(sysfs_mount_t));

    fs_register("sysfs", &sysfs_superblock_ops, sysfs_mount, sysfs_unmount);

    return ERROR_OK;
}

error_number_t sysfs_add_entry(const char * path, sysfs_id_t id, sysfs_read_op_t * read_op, sysfs_write_op_t * write_op) {
    sysfs_entry_t * entry = heap_alloc(sizeof(sysfs_entry_t));

    entry->path = heap_alloc(strlen(path) + 1);
    strcpy(entry->path, path);

    entry->id = id;
    entry->read = read_op;
    entry->write = write_op;

    entry->prev = &sysfs_head;
    entry->next = sysfs_head.next;

    sysfs_head.next->prev = entry;
    sysfs_head.next = entry;

    for (uint64_t i = 0; i < sysfs_mount_count; i++) {
        fs_directory_entry_t * dirent = fs_make_path_dirs(sysfs_mounts[i].mount_point, path, FS_REGULAR);

        if (dirent == NULL) return ERROR_EXISTS;

        sysfs_fs_node_t * node = (sysfs_fs_node_t *) dirent->node;

        node->id = id;
        node->read = read_op;
        node->write = write_op;
    }

    return ERROR_OK;
}

error_number_t sysfs_remove_entry(const char * path) {
    for (sysfs_entry_t * entry = sysfs_head.next; entry != &sysfs_tail; entry = entry->next) {
        if (strcmp(entry->path, path) == 0) {
            entry->next->prev = entry->prev;
            entry->prev->next = entry->next;

            heap_free(entry->path);
            heap_free(entry);

            break;
        }
    }

    for (uint64_t i = 0; i < sysfs_mount_count; i++) {
        fs_directory_entry_t * dirent = fs_open_path(sysfs_mounts[i].mount_point, path);
        if (dirent == NULL) return ERROR_FS_NO_ENT;

        while (true) {
            fs_directory_entry_t * parent = dirent->parent;
            fs_directory_entry_add_reference(parent);

            fs_directory_entry_release(dirent);
            if (fs_remove(dirent) != ERROR_OK) break;

            if (parent == NULL) break;

            dirent = parent;
        }
    }



    return ERROR_UNIMPLEMENTED;
}

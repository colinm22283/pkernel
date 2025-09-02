#include <stdbool.h>
#include <stddef.h>

#include <filesystem/ramfs/ramfs.h>

#include <filesystem/filesystem_table.h>

filesystem_table_entry_t * filesystem_entry;

bool ramfs_init(void) {
    superblock_operations_t superblock_operations = {
        .create_node = ramfs_create_node,
        .delete_node = ramfs_delete_node,
        .unmount = ramfs_unmount,
    };

    filesystem_entry = filesystem_table_push("ramfs", ramfs_mount, &superblock_operations);
    if (filesystem_entry == NULL) return false;

    return true;
}
#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

#include <device/device.h>

#include <error_number.h>

typedef error_number_t (* fs_mount_func_t)(fs_superblock_t * superblock);
typedef error_number_t (* fs_unmount_func_t)(fs_superblock_t * superblock);

typedef struct fs_filesystem_node_s {
    char * name;

    uint64_t mount_count;

    fs_mount_func_t mount;
    fs_unmount_func_t unmount;
    const fs_superblock_ops_t * superblock_ops;

    struct fs_filesystem_node_s * next;
    struct fs_filesystem_node_s * prev;
} fs_filesystem_node_t;

extern fs_directory_entry_t fs_root;

bool fs_init();

error_number_t fs_register(const char * name, const fs_superblock_ops_t * superblock_ops, fs_mount_func_t mount, fs_unmount_func_t unmount);
error_number_t fs_unregister(const char * name);

error_number_t fs_mount_root(const char * name, device_t * device);
error_number_t fs_mount(const char * name, fs_directory_entry_t * mount_point, device_t * device);
error_number_t fs_unmount(fs_directory_entry_t * mount_point);

fs_directory_entry_t * fs_make(fs_directory_entry_t * parent, const char * name, fs_file_type_t type);
fs_directory_entry_t * fs_make_anon(fs_file_type_t type);

fs_directory_entry_t * fs_open_path(fs_directory_entry_t * root, const char * path);
fs_directory_entry_t * fs_make_path(fs_directory_entry_t * root, const char * path, fs_file_type_t type);

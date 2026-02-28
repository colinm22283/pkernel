#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

#include <device/device.h>

#include <errno.h>

typedef int (* fs_mount_func_t)(fs_superblock_t * superblock);
typedef int (* fs_unmount_func_t)(fs_superblock_t * superblock);

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

int fs_register(const char * name, const fs_superblock_ops_t * superblock_ops, fs_mount_func_t mount, fs_unmount_func_t unmount);
int fs_unregister(const char * name);

int fs_mount_root(const char * name, device_t * device);
int fs_mount(const char * name, fs_directory_entry_t * mount_point, device_t * device);
int fs_unmount(fs_directory_entry_t * mount_point);

fs_directory_entry_t * fs_make(fs_directory_entry_t * parent, const char * name, fs_file_type_t type);
fs_directory_entry_t * fs_make_anon(fs_file_type_t type);
fs_directory_entry_t * fs_make_anon_pipe(void);
fs_directory_entry_t * fs_make_anon_socket(socket_domain_t domain, socket_type_t type, uint64_t protocol);

fs_directory_entry_t * fs_open_path(fs_directory_entry_t * root, const char * path);
fs_directory_entry_t * fs_make_path(fs_directory_entry_t * root, const char * path, fs_file_type_t type);
fs_directory_entry_t * fs_make_path_dirs(fs_directory_entry_t * root, const char * path, fs_file_type_t type);

int fs_remove(fs_directory_entry_t * dirent);

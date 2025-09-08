#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/directory_entry.h>

#include <device/device.h>

struct fs_superblock_s;

typedef struct {
    fs_node_t * (* alloc_node)(struct fs_superblock_s * superblock);
    error_number_t (* free_node)(struct fs_superblock_s * superblock, fs_node_t * node);

    error_number_t (* list)(struct fs_directory_entry_s * dirent);
    error_number_t (* lookup)(struct fs_directory_entry_s * dirent, struct fs_directory_entry_node_s * dirent_node, struct fs_node_s * node);

    fs_directory_entry_node_t * (* create)(struct fs_directory_entry_s * parent, struct fs_node_s * _node, const char * name, fs_file_type_t type);
    fs_directory_entry_node_t * (* link)(struct fs_directory_entry_s * dirent, struct fs_directory_entry_s * subdirent, const char * name);
    error_number_t (* unlink)(struct fs_node_s * node, struct fs_node_s * subnode, const char * name);

    error_number_t (* read)(struct fs_directory_entry_s * dirent, char * data, fs_size_t size, fs_size_t offset, fs_size_t * read);
    error_number_t (* write)(struct fs_directory_entry_s * dirent, const char * data, fs_size_t size, fs_size_t offset, fs_size_t * wrote);
} fs_superblock_ops_t;

typedef struct fs_superblock_s {
    fs_directory_entry_t * prev_dirent;
    fs_directory_entry_t * mount_point;
    device_t * device;

    const fs_superblock_ops_t * superblock_ops;

    void * private;
} fs_superblock_t;

fs_superblock_t * superblock_alloc(const fs_superblock_ops_t * superblock_ops);
void superblock_free(fs_superblock_t * superblock);

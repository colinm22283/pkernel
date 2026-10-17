#pragma once

#include <stdint.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

#include <filesystem/sysfs/sysfs.h>

#include <errno.h>

typedef struct {
    fs_node_t base;

    fs_sysfs_id_t id;
    fs_sysfs_read_op_t * read;
    fs_sysfs_write_op_t * write;
} fs_sysfs_fs_node_t;

typedef struct {
    fs_directory_entry_t * mount_point;
} fs_sysfs_mount_t;

typedef struct fs_sysfs_entry_s {
    char * path;

    fs_sysfs_id_t id;
    fs_sysfs_read_op_t * read;
    fs_sysfs_write_op_t * write;

    struct fs_sysfs_entry_s * next;
    struct fs_sysfs_entry_s * prev;
} fs_sysfs_entry_t;

extern fs_sysfs_entry_t sysfs_head, sysfs_tail;

extern uint64_t sysfs_mount_count;
extern fs_sysfs_mount_t * sysfs_mounts;

extern const fs_superblock_ops_t sysfs_superblock_ops;

int fs_sysfs_mount(fs_superblock_t * superblock);
int fs_sysfs_unmount(fs_superblock_t * superblock);



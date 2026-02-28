#pragma once

#include <stdint.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

#include <sysfs/sysfs.h>

#include <errno.h>

typedef struct {
    fs_node_t base;

    sysfs_id_t id;
    sysfs_read_op_t * read;
    sysfs_write_op_t * write;
} sysfs_fs_node_t;

typedef struct {
    fs_directory_entry_t * mount_point;
} sysfs_mount_t;

typedef struct sysfs_entry_s {
    char * path;

    sysfs_id_t id;
    sysfs_read_op_t * read;
    sysfs_write_op_t * write;

    struct sysfs_entry_s * next;
    struct sysfs_entry_s * prev;
} sysfs_entry_t;

extern sysfs_entry_t sysfs_head, sysfs_tail;

extern uint64_t sysfs_mount_count;
extern sysfs_mount_t * sysfs_mounts;

extern const fs_superblock_ops_t sysfs_superblock_ops;

int sysfs_mount(fs_superblock_t * superblock);
int sysfs_unmount(fs_superblock_t * superblock);


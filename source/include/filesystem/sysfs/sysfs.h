#pragma once

#include <errno.h>

typedef uint64_t fs_sysfs_id_t;

typedef int64_t (fs_sysfs_read_op_t)(uint64_t id, char * data, uint64_t size, uint64_t offset);
typedef int64_t (fs_sysfs_write_op_t)(uint64_t id, const char * data, uint64_t size, uint64_t offset);

int fs_sysfs_init(void);

int fs_sysfs_add_entry(const char * path, fs_sysfs_id_t id, fs_sysfs_read_op_t * read_op, fs_sysfs_write_op_t * write_op);
int fs_sysfs_remove_entry(const char * path);


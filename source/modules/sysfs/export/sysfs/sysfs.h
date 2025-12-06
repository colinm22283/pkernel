#pragma once

#include <error_number.h>

typedef uint64_t sysfs_id_t;

typedef int64_t (sysfs_read_op_t)(uint64_t id, char * data, uint64_t size, uint64_t offset);
typedef int64_t (sysfs_write_op_t)(uint64_t id, const char * data, uint64_t size, uint64_t offset);

error_number_t sysfs_add_entry(const char * path, sysfs_id_t id, sysfs_read_op_t * read_op, sysfs_write_op_t * write_op);
error_number_t sysfs_remove_entry(const char * path);

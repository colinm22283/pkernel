#pragma once

#include <device/device.h>

#include <filesystem/node.h>

#include <error_number.h>

typedef struct devfs_entry_s {
    char * name;
    device_t * device;

    struct devfs_entry_s * next;
    struct devfs_entry_s * prev;
} devfs_entry_t;

error_number_t devfs_init(void);

devfs_entry_t * devfs_register(device_t * device);
error_number_t devfs_remove(devfs_entry_t * entry);

device_t * devfs_open(fs_node_t * devfs_node);
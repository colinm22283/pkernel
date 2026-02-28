#pragma once

#include <device/device.h>

#include <filesystem/node.h>

#include <errno.h>

typedef struct devfs_entry_s {
    char * name;
    device_t * device;

    struct devfs_entry_s * next;
    struct devfs_entry_s * prev;
} devfs_entry_t;

int devfs_init(void);

devfs_entry_t * devfs_register(device_t * device);
int devfs_remove(devfs_entry_t * entry);
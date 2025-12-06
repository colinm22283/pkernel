#pragma once

#include <stdbool.h>

#include <sys/ata/pio.h>

typedef struct {
    port_t io_port;
    port_t control_port;

    bool is_master;

    devfs_entry_t * devfs_entry;
} ide_device_t;

extern uint64_t ide_device_count;
extern ide_device_t ** ide_devices;

extern device_block_operations_t block_ops;
extern device_block_data_t block_data;

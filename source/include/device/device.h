#pragma once

#include <stdint.h>

#include <device/operations.h>

#include <event/event.h>

typedef enum {
    DT_CHARACTER,
    DT_BLOCK,
} device_type_t;

typedef struct {
} device_char_data_t;

typedef struct {
    uint64_t block_size;
} device_block_data_t;

typedef struct device_s {
    device_type_t type;

    event_t * read_ready;

    char * name;
    void * private;

    union {
        device_char_operations_t char_ops;
        device_block_operations_t block_ops;
    };

    union {
        device_char_data_t char_data;
        device_block_data_t block_data;
    };
} device_t;

bool device_init(void);

device_t * device_create_char(const char * name, void * private, device_char_operations_t * operations, device_char_data_t * data);
device_t * device_create_block(const char * name, void * private, device_block_operations_t * operations, device_block_data_t * data);

bool device_remove(device_t * device);

uint64_t device_read(device_t * device, char * buffer, uint64_t size, uint64_t offset);
uint64_t device_write(device_t * device, const char * buffer, uint64_t size, uint64_t offset);

#pragma once

#include <device/device.h>
#include <devfs/devfs.h>

#include <defs.h>

struct input_device_s;

#define IT_KEY (1 << 0)
#define IT_REL (1 << 1)
#define IT_ABS (1 << 2)
typedef uint32_t input_type_t;

enum {
    IT_ACTION_PRESS   = 0,
    IT_ACTION_RELEASE = 1,
    IT_ACTION_REPEAT  = 2,
};

enum {
    IT_REL_X = 0,
    IT_REL_Y = 1,
};

typedef struct __PACKED {
    input_type_t type;
    uint8_t size;
} input_packet_header_t;

typedef struct __PACKED {
    uint8_t  action;
    uint16_t code;
} input_packet_key_t;

typedef struct __PACKED {
    uint8_t axis;
    int32_t value;
} input_packet_rel_t;

typedef struct __PACKED {
    input_packet_header_t header;

    union {
        input_packet_key_t key;
        input_packet_rel_t rel;
    } data;
} input_packet_t;

typedef size_t input_write_handler_t(struct input_device_s *, input_packet_t *, size_t);

typedef struct input_device_s {
    device_t * device;
    devfs_entry_t * devfs_entry;

    event_t * read_ready;

    size_t size, capacity;
    size_t head, tail;
    input_packet_t * packets;
} input_device_t;

void input_init(void);

input_device_t * input_add_device(input_write_handler_t * write_handler, void * cookie);
void input_free_device(input_device_t * device);

size_t input_provide_packet(input_device_t * device, input_packet_t * packets);


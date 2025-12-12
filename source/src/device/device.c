#include <stddef.h>

#include <device/device.h>

#include <util/heap/heap.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/memory/memcpy.h>

#include "debug/vga_print.h"

typedef struct device_node_s {
    device_t device;

    struct device_node_s * next;
    struct device_node_s * prev;
} device_node_t;

device_node_t devices_head, devices_tail;

bool device_init(void) {
    devices_head.next = &devices_tail;
    devices_head.prev = NULL;
    devices_tail.next = NULL;
    devices_tail.prev = &devices_head;

    return true;
}

device_t * device_create_char(const char * name, void * private, device_char_operations_t * operations, device_char_data_t * data) {
    device_node_t * new_node = heap_alloc_debug(sizeof(device_node_t), "device char node");

    new_node->device.type = DT_CHARACTER;
    new_node->device.name = heap_alloc_debug(strlen(name) + 1, "device char node name");
    strcpy(new_node->device.name, name);
    new_node->device.private = private;

    memcpy(&new_node->device.char_ops, operations, sizeof(device_char_operations_t));
    memcpy(&new_node->device.char_data, data, sizeof(device_char_data_t));

    new_node->device.read_ready = event_init();

    new_node->next = devices_head.next;
    new_node->prev = &devices_head;
    devices_head.next->prev = new_node;
    devices_head.next = new_node;

    return &new_node->device;
}

device_t * device_create_block(const char * name, void * private, device_block_operations_t * operations, device_block_data_t * data) {
    device_node_t * new_node = heap_alloc_debug(sizeof(device_node_t), "device block node");

    new_node->device.type = DT_BLOCK;
    new_node->device.name = heap_alloc_debug(strlen(name) + 1, "device block node name");
    strcpy(new_node->device.name, name);
    new_node->device.private = private;

    memcpy(&new_node->device.block_ops, operations, sizeof(device_block_operations_t ));
    memcpy(&new_node->device.block_data, data, sizeof(device_block_data_t));

    new_node->device.read_ready = event_init();

    new_node->next = devices_head.next;
    new_node->prev = &devices_head;
    devices_head.next->prev = new_node;
    devices_head.next = new_node;

    return &new_node->device;
}

bool device_remove(device_t * device) {
    device_node_t * node = devices_head.next;

    while (node != &devices_tail) {
        if (&node->device == device) {
            // TODO

            return true;
        }
    }

    return false;
}

uint64_t device_read(device_t * device, char * buffer, uint64_t size, uint64_t offset) {
    switch (device->type) {
        case DT_CHARACTER: {
            return device->char_ops.read(device, buffer, size);
        } break;

        case DT_BLOCK: {
            return device->block_ops.read(device, buffer, size / device->block_data.block_size, offset / device->block_data.block_size);
        } break;
    }

    return 0;
}

uint64_t device_write(device_t * device, const char * buffer, uint64_t size, uint64_t offset) {
    switch (device->type) {
        case DT_CHARACTER: return device->char_ops.write(device, buffer, size);

        case DT_BLOCK: {
            return device->block_ops.write(device, buffer, size / device->block_data.block_size, offset / device->block_data.block_size);
        } break;
    }

    return 0;
}

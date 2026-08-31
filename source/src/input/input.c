#include <input/input.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>
#include <util/string/writestr.h>
#include <util/memory/memcpy.h>

#include <config/input.h>

#ifdef INPUT_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("input");

size_t input_number;

void input_init(void) {
    input_number = 0;
}

uint64_t input_write(device_t * dev, const char * buffer, uint64_t size) {
    /* input_device_t * device = dev->private; */

    size_t offset = 0;
    while (true) {
        if (size - offset < sizeof(input_packet_header_t)) break;

        input_packet_header_t * header = (input_packet_header_t *) (buffer + offset);

        if (size - offset < sizeof(input_packet_header_t) + header->size) break;

        input_packet_t * packet = (input_packet_t *) (buffer + offset);

        bool accepted = false;

        switch (packet->header.type) {
            case IT_KEY: {
                if (
                    packet->header.size !=
                    sizeof(input_packet_header_t) + sizeof(input_packet_key_t)
                ) {

                }
            } break;

            default: break;
        }

        if (!accepted) {
            break;
        }

        offset += packet->header.size;
    }

    return 0;
}

uint64_t input_read(device_t * dev, char * buffer, uint64_t size) {
    input_device_t * device = dev->private;

    while (device->size == 0) {
        scheduler_await(device->read_ready);
    }

    input_packet_t * packet = &device->packets[device->tail];

    if (packet->header.size > size) return 0;

    memcpy(buffer, packet, packet->header.size);

    device->tail = (device->tail + 1) % device->capacity;
    device->size--;

    return packet->header.size;
}

input_device_t * input_add_device(input_write_handler_t * write_handler, void * cookie) {
    device_char_data_t dev_data = { };

    device_char_operations_t dev_ops = {
        .write = input_write,
        .read = input_read,
    };

    input_device_t * device = heap_alloc(sizeof(input_device_t));

    device->read_ready = event_init();

    device->size = 0;
    device->capacity = INPUT_BUFFER_SIZE;
    device->head = 0;
    device->tail = 0;
    device->packets = heap_alloc(device->capacity * sizeof(input_packet_t));

    char name[100] = "input";
    name[writestr(name + 5, 99 - 5, 0, input_number) + 5] = '\0';

    device->device = device_create_char(name, device, &dev_ops, &dev_data);
    devfs_register(device->device);

    return device;
}

void input_free_device(input_device_t * device) {
}

size_t input_provide_packet(input_device_t * device, input_packet_t * packet) {
    if (device->size == device->capacity) return 0;
    
    input_packet_t * dst = &device->packets[device->head];

    memcpy(dst, packet, packet->header.size);

    device->head = (device->head + 1) % device->capacity;
    device->size++;

    event_invoke(device->read_ready);

    return 1;
}


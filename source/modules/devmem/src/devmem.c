#include <stdbool.h>

#include <device/device.h>
#include <device/devfs.h>

#include <util/heap/heap.h>

device_t * device;
filesystem_node_t * devfs_node;

uint64_t write(device_t * dev, const char * buffer, uint64_t size) { return 0; }
uint64_t read(device_t * dev, char * buffer, uint64_t size) { return 0; }

bool map(device_t * dev, void ** private, pml4t64_t * pml4t, void * map_addr, uint64_t size, uint64_t offset) {
    paging_mapping_t * new_mapping = heap_alloc(sizeof(paging_mapping_t));

    if (!paging_map_ex(pml4t, new_mapping, offset, map_addr, DIV_UP(size, 0x1000), true, true, true)) {
        heap_free(new_mapping);

        return false;
    }

    *private = new_mapping;

    return true;
}

bool unmap(device_t * dev, void * private) {
    paging_unmap(private);

    heap_free(private);

    return true;
}

bool init() {
    device_char_operations_t operations = {
        .write = write,
        .read = read,
        .map = map,
        .unmap = unmap,
    };
    device_char_data_t data = { };
    device = device_create_char("mem", NULL, &operations, &data);
    devfs_node = devfs_register(device);

    return true;
}

bool free() {
    devfs_unregister(devfs_node);
    device_remove(device);
}
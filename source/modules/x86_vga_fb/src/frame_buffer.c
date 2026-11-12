#include <stddef.h>

#include <device/device.h>

#include <devfs/devfs.h>

#include <pman/pman.h>
#include <pman/source.h>
#include <pman/mapping.h>

#include <debug/printf.h>

#include <util/math/div_up.h>

#include <mod_defs.h>

#define FRAME_BUFFER_PADDR (0xA0000)
#define FRAME_BUFFER_SIZE_BYTES (320 * 200)
#define FRAME_BUFFER_SIZE_PAGES (DIV_UP(FRAME_BUFFER_SIZE_BYTES, PAGE_SIZE))

device_t * device;
devfs_entry_t * devfs_entry;

pman_source_t * sources[FRAME_BUFFER_SIZE_PAGES];

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    return 0;
}
uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    return 0;
}

void * fb_map(device_t * dev, pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t size, uint64_t offset) {
    if (size > FRAME_BUFFER_SIZE_BYTES) return NULL;

    pman_range_t * range = pman_add_anon_map_sources(
        context,
        sources,
        FRAME_BUFFER_SIZE_PAGES,
        vaddr,
        0,
        prot
    );

    printf("Test: %p\n", range->vaddr);

    return range->vaddr;
}

int fb_unmap(device_t * dev, pman_context_t * context, void * map_addr) {
    return 0;
}

int init(void) {
    for (size_t i = 0; i < FRAME_BUFFER_SIZE_PAGES; i++) {
        sources[i] = pman_source_init_anon_kernel(FRAME_BUFFER_PADDR + i * PAGE_SIZE);
    }

    device_char_operations_t operations = {
        .read = read,
        .write = write,
        .map = fb_map,
        .unmap = fb_unmap,
    };
    device_char_data_t data = { };
    device = device_create_char("vga", NULL, &operations, &data);
    devfs_entry = devfs_register(device);

    return 0;
}

int free(void) {
    devfs_remove(devfs_entry);
    device_remove(device);

    return 0;
}

MODULE_NAME("x86_vga_fb");
MODULE_DEPS("devfs");


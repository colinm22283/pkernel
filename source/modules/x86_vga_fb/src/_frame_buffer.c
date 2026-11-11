#include <stddef.h>

#include <paging/mapper.h>
#include <paging/manager.h>
#include <paging/virtual_allocator.h>

#include <device/device.h>

#include <devfs/devfs.h>

#include <util/heap/heap.h>
#include <util/math/div_up.h>
#include <sys/paging/page_size.h>

#include <mod_defs.h>
#include <entry_error.h>

#define FRAME_BUFFER_PADDR (0xA0000)
#define FRAME_BUFFER_SIZE_BYTES (320 * 200)
#define FRAME_BUFFER_SIZE_PAGES (DIV_UP(FRAME_BUFFER_SIZE_BYTES, PAGE_SIZE))

device_t * device;
devfs_entry_t * devfs_entry;

pman_mapping_t * pman_entry;

uint64_t required_pages;
uint8_t * frame_buffer;
paging_mapping_t mapping;

bool vga_get_framebuffer(void ** fb) {
    if (fb == NULL) return false;

    *fb = frame_buffer;

    return true;
}

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    return 0;
}
uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    return 0;
}

void * map(device_t * dev, pman_context_t * context, pman_protection_flags_t prot, void * map_addr, uint64_t size, uint64_t offset) {
    pman_mapping_t * temp_mapping = pman_context_add_map(pman_kernel_context(), PMAN_PROT_WRITE, NULL, FRAME_BUFFER_PADDR, FRAME_BUFFER_SIZE_BYTES);
    pman_mapping_t * mapping = pman_context_add_shared(context, PMAN_PROT_WRITE, temp_mapping, map_addr);
    pman_context_unmap(temp_mapping);

    return mapping->vaddr;
}

int unmap(device_t * dev, pman_context_t * context, void * map_addr) {


    return 0;
}

int init(void) {
    required_pages = FRAME_BUFFER_SIZE_PAGES;

    pman_entry = pman_context_add_map(pman_kernel_context(), PMAN_PROT_WRITE, NULL, FRAME_BUFFER_PADDR, 320 * 200);

//    frame_buffer = (void *) 0x10000000;
    frame_buffer = pman_entry->vaddr;

//    paging_map(&mapping, FRAME_BUFFER_PADDR, frame_buffer, required_pages);
    device_char_operations_t operations = {
        .write = write,
        .read = read,
        .map = map,
        .unmap = unmap,
    };
    device_char_data_t data = { };
    device = device_create_char("vga", NULL, &operations, &data);
    devfs_entry = devfs_register(device);

    return 0;
}

int free(void) {
    devfs_remove(devfs_entry);
    device_remove(device);

    paging_unmap(&paging_kernel_pml4t, &mapping);
//    paging_valloc_free(frame_buffer, required_pages);

    return 0;
}

MODULE_NAME("vga_fb");
MODULE_DEPS("devfs");

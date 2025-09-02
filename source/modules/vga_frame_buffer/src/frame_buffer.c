#include <stddef.h>

#include <paging/mapper.h>
#include <paging/manager.h>
#include <paging/virtual_allocator.h>

#include <device/devfs.h>
#include <device/device.h>

#include <interface/interface_map.h>

#include <util/heap/heap.h>
#include <util/math/div_up.h>

#include <entry_error.h>

#define FRAME_BUFFER_PADDR (0xA0000)
#define FRAME_BUFFER_SIZE_BYTES (320 * 200)
#define FRAME_BUFFER_SIZE_PAGES (DIV_UP(FRAME_BUFFER_SIZE_BYTES, 0x1000))

device_t * device;
devfs_entry_t * devfs_entry;

pman_mapping_t * pman_entry;

interface_map_node_t * interface_node;

uint64_t required_pages;
uint8_t * frame_buffer;
paging_mapping_t mapping;

uint8_t primary_color, secondary_color;

bool vga_set_primary(const uint8_t * color) {
    primary_color = *color;

    return true;
}

bool vga_set_secondary(const uint8_t * color) {
    secondary_color = *color;

    return true;
}

bool vga_clear(void) {
    for (int i = 0; i < 320 * 200; i++) frame_buffer[i] = secondary_color;

    return true;
}

bool vga_fill_rect(uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    for (uint64_t x = _x; x < _x + w; x++) {
        for (uint64_t y = _y; y < _y + h; y++) {
            frame_buffer[y * 320 + x] = primary_color;
        }
    }

    return true;
}

bool vga_clear_rect(uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    for (uint64_t x = _x; x < _x + w; x++) {
        for (uint64_t y = _y; y < _y + h; y++) {
            frame_buffer[y * 320 + x] = secondary_color;
        }
    }

    return true;
}

bool draw_bitmap(const uint8_t * bitmap, uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    if (bitmap == NULL) return false;

    uint64_t byte_width = w / 8;

    for (uint64_t y = 0; y < h; y++) {
        for (uint64_t x = 0; x < w; x++) {
            frame_buffer[(_y + y) * 320 + _x + x] = ((bitmap[y * byte_width + x / 8] >> x) & 1) ? primary_color : secondary_color;
        }
    }

    return true;
}

bool draw_bitmap_transparent(const uint8_t * bitmap, uint64_t _x, uint64_t _y, uint64_t w, uint64_t h) {
    if (bitmap == NULL) return false;

    uint64_t byte_width = w / 8;

    for (uint64_t y = 0; y < h; y++) {
        for (uint64_t x = 0; x < w; x++) {
            if ((bitmap[y * byte_width + x / 8] >> x) & 1) frame_buffer[(_y + y) * 320 + _x + x] = primary_color;
        }
    }

    return true;
}

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
    pman_mapping_t * temp_mapping = pman_context_add_map(context, PMAN_PROT_WRITE, NULL, FRAME_BUFFER_PADDR, FRAME_BUFFER_SIZE_BYTES);
    pman_mapping_t * mapping = pman_context_add_shared(context, PMAN_PROT_WRITE, temp_mapping, map_addr);
    pman_context_unmap(temp_mapping);

    return mapping->vaddr;
}

error_number_t unmap(device_t * dev, pman_context_t * context, void * map_addr) {


    return ERROR_OK;
}

bool init(void) {
    required_pages = DIV_UP(320 * 200, 0x1000);

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

    interface_node = interface_map_add("vga");

    interface_add_method(&interface_node->interface, "set_primary", vga_set_primary);
    interface_add_method(&interface_node->interface, "set_secondary", vga_set_secondary);
    interface_add_method(&interface_node->interface, "clear", vga_clear);

    interface_add_method(&interface_node->interface, "fill_rect", vga_fill_rect);
    interface_add_method(&interface_node->interface, "clear_rect", vga_clear_rect);

    interface_add_method(&interface_node->interface, "draw_bitmap", draw_bitmap);
    interface_add_method(&interface_node->interface, "draw_bitmap_transparent", draw_bitmap_transparent);

    interface_add_method(&interface_node->interface, "get_frame_buffer", vga_get_framebuffer);

    return true;
}

bool free(void) {
    devfs_remove(devfs_entry);
    device_remove(device);

    interface_map_remove(interface_node);

    paging_unmap(&paging_kernel_pml4t, &mapping);
//    paging_valloc_free(frame_buffer, required_pages);

    return true;
}
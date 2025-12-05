#include <stddef.h>
#include <stdbool.h>

#include <interface/interface_map.h>

#include <paging/manager.h>

#include <timer/timer.h>

#include <device/devfs.h>
#include <device/device.h>

#include <sysfs/sysfs.h>

#include <vga_terminal.h>
#include <font.h>
#include <update.h>

#include <mod_defs.h>

__MOD_EXPORT device_t * vga_term_device; // TODO: delete this

device_t * device;
devfs_entry_t * devfs_entry;

timer_t * blink_timer;

bool (* draw_bitmap)(uint8_t * bitmap, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
bool (* draw_bitmap_transparent)(uint8_t * bitmap, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
bool (* clear_rect)(uint64_t x, uint64_t y, uint64_t w, uint64_t h);

volatile bool bound = true;
volatile bool cursor_on;
uint8_t cur_x, cur_y, view_y, buffer_start;

console_buffer_t * console_buffer;

void timer_handler(timer_t * timer) {
    cursor_on = !cursor_on;

    terminal_update();
}

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    if (size > 0) cursor_on = true;

    for (uint64_t i = 0; i < size; i++) {
        if (buffer[i] == '\n') {
            cur_y = (cur_y + 1) % BUFFER_HEIGHT;
            cur_x = 0;

            if (cur_y == buffer_start) {
                for (uint64_t j = 0; j < WINDOW_WIDTH; j++) CONSOLE_BUFFER[j][buffer_start] = ' ';
                buffer_start = (buffer_start + 1) % BUFFER_HEIGHT;
            }

            view_y = cur_y;
        }
        else if (buffer[i] == '\r') {
            cur_x = 0;
        }
        else if (buffer[i] == 0x08) { // backspace
            cur_x = (cur_x - 1 + WINDOW_WIDTH) % WINDOW_WIDTH;
        }
        else {
            if (cur_x < WINDOW_WIDTH) {
                CONSOLE_BUFFER[cur_x][cur_y] = buffer[i];

                cur_x++;
            }

            view_y = cur_y;
        }
    }

    terminal_update();

    return size;
}
uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    return 0;
}

enum {
    SYSFS_BIND = 0,
};

int64_t sysfs_read(uint64_t id, char * data, uint64_t size, uint64_t offset) {
    if (offset != 0) return 0;

    switch (id) {
        case SYSFS_BIND: {
            if (size > 0) {
                if (bound) data[0] = '1';
                else data[0] = '0';

                return 1;
            }
        }

        default: break;
    }
    return 0;
}

int64_t sysfs_write(uint64_t id, const char * data, uint64_t size, uint64_t offset) {
    switch (id) {
        case SYSFS_BIND: {
            if (size == 1) {
                if (data[0] == '1') {
                    bound = true;
                    terminal_update();

                    return 1;
                }
                else if (data[0] == '0') {
                    bound = false;

                    return 1;
                }
            }
        } break;

        default: break;
    }

    return 0;
}

bool init(void) {
    cursor_on = true;
    cur_x = 0;
    cur_y = 0;
    view_y = cur_y;
    buffer_start = 1;

    pman_mapping_t * buffer_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, sizeof(console_buffer_t));

    console_buffer = buffer_mapping->vaddr;

    interface_t * vga_interface = interface_map_lookup("vga");
    if (vga_interface == NULL) return false;

    draw_bitmap = interface_lookup_method(vga_interface, "draw_bitmap");
    if (draw_bitmap == NULL) return false;
    bool (* vga_set_primary)(const uint8_t * color) = interface_lookup_method(vga_interface, "set_primary");
    bool (* vga_set_secondary)(const uint8_t * color) = interface_lookup_method(vga_interface, "set_secondary");
    bool (* vga_clear)(void) = interface_lookup_method(vga_interface, "clear");

    draw_bitmap_transparent = interface_lookup_method(vga_interface, "draw_bitmap_transparent");
    if (draw_bitmap_transparent == NULL) return false;

    clear_rect = interface_lookup_method(vga_interface, "clear_rect");
    if (clear_rect == NULL) return false;

    // blink_timer = timer_init(timer_handler, NULL, 0, TIMER_MS_TO_TICKS(500));

    {
        uint8_t color = 15;
        vga_set_primary(&color);
    }
    {
        uint8_t color = 0;
        vga_set_secondary(&color);
    }
    vga_clear();

    device_char_operations_t operations = {
        .write = write,
        .read = read,
    };
    device_char_data_t data = { };
    device = device_create_char("tty", NULL, &operations, &data);
    if (device == NULL) return false;
    devfs_entry = devfs_register(device);
    if (devfs_entry == NULL) return false;

    sysfs_add_entry("vgatty/bind", SYSFS_BIND, sysfs_read, sysfs_write);

    vga_term_device = device;

    return true;
}

bool free(void) {
    devfs_remove(devfs_entry);
    device_remove(device);

    timer_free(blink_timer);

//    paging_free(&buffer_allocation);

    return true;
}
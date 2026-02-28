#include <stddef.h>
#include <stdbool.h>

#include <paging/manager.h>

#include <timer/timer.h>

#include <device/device.h>

#include <devfs/devfs.h>

#include <sysfs/sysfs.h>

#include <util/memory/memset.h>

#include <vga_terminal.h>
#include <font.h>
#include <update.h>

#include <mod_defs.h>

__MOD_EXPORT device_t * vga_term_device; // TODO: delete this

uint8_t * frame_buffer;

device_t * device;
devfs_entry_t * devfs_entry;

timer_t * blink_timer;

volatile bool bound = true;
volatile bool cursor_on;
uint8_t cur_x, cur_y, view_y, buffer_start;

pman_mapping_t * buffer_mapping;
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

int init(void) {
    cursor_on = true;
    cur_x = 0;
    cur_y = 0;
    view_y = cur_y;
    buffer_start = 1;

    frame_buffer = pman_context_add_map(pman_kernel_context(), PMAN_PROT_WRITE, NULL, 0xA0000, WINDOW_WIDTH * 8 * WINDOW_HEIGHT * 8)->vaddr;

    buffer_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, sizeof(console_buffer_t));
    memset(buffer_mapping->vaddr, 0, sizeof(console_buffer_t));

    console_buffer = buffer_mapping->vaddr;

    blink_timer = timer_init(timer_handler, NULL, 0, TIMER_MS_TO_TICKS(500));

    device_char_operations_t operations = {
        .write = write,
        .read = read,
    };
    device_char_data_t data = { };
    device = device_create_char("tty", NULL, &operations, &data);
    if (device == NULL) return ERROR_UNKNOWN;
    devfs_entry = devfs_register(device);
    if (devfs_entry == NULL) return ERROR_UNKNOWN;

    sysfs_add_entry("vgatty/bind", SYSFS_BIND, sysfs_read, sysfs_write);

    vga_term_device = device;

    return ERROR_OK;
}

int free(void) {
    devfs_remove(devfs_entry);
    device_remove(device);

    timer_free(blink_timer);

    pman_context_unmap(buffer_mapping);

    return ERROR_OK;
}

MODULE_NAME("vga_tty");
MODULE_DEPS("vga_fb", "sysfs", "devfs");
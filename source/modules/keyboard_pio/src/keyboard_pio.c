#include <stddef.h>

#include <interrupt/interrupt_registry.h>
#include <interface/interface_map.h>

#include <io/arbitrator.h>

#include <device/device.h>
#include <device/devfs.h>

#include <process/scheduler.h>

#include <sys/ports.h>
#include <sys/asm/in.h>
#include <sys/interrupt/interrupt_code.h>

#include <key_lut.h>

#include <debug/vga_print.h>

device_t * device;

volatile bool char_ready;
volatile char current_char;

bool keyboard_handler(interrupt_code_t channel, void * cookie) {
    unsigned char input_char = (unsigned char) inb(PORT_KB_IN);
    bool released = input_char >= 0x80;

    if (!released) {
        char translated_char = key_lut[input_char];

        char_ready = true;
        current_char = (char) translated_char;

        event_invoke_once(device->read_ready);
    }

    return true;
}

bool get_char(char * c) {
    while (!char_ready);

    char_ready = false;
    *c = current_char;

    return true;
}

bool has_char(void) {
    return char_ready;
}

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    return 0;
}
uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    if (!has_char()) {
        event_await(scheduler_current_thread(), dev->read_ready);
    }

    if (has_char() && size > 0) {
        get_char(buffer);

        return 1;
    }
    else return 0;
}

bool init(void) {
    char_ready = true;
    current_char = '\0';

    device_char_operations_t operations = {
        .write = write,
        .read = read,
    };
    device_char_data_t data = { };
    device = device_create_char("kbd", NULL, &operations, &data);
    devfs_register(device);

    if (!io_arbitrator_reserve(PORT_KB_IN)) return false;
    inb(PORT_KB_IN);

    if (!interrupt_registry_register(IC_KEYBOARD, keyboard_handler, NULL)) return false;

    return true;
}

bool free(void) {
    if (!interrupt_registry_free(IC_KEYBOARD)) return false;

    if (!io_arbitrator_release(PORT_KB_IN)) return false;

    device_remove(device);

    return true;
}
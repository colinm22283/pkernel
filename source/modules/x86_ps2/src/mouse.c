#include <interrupt/interrupt_registry.h>

#include <device/device.h>

#include <scheduler/scheduler.h>

#include <devfs/devfs.h>

#include <util/memory/memcpy.h>

#include <sys/interrupt/interrupt_code.h>
#include <sys/pic/pic.h>

#include <errno.h>

#include <debug/printf.h>

#include <mod_defs.h>

#define PS2_DATA    (0x60)
#define PS2_STATUS  (0x64)
#define PS2_COMMAND (0x64)

device_t * device;

static inline void wait_data(void) {
    while (!(inb(PS2_STATUS) & 0b1));
}

static inline void wait_write(void) {
    while (inb(PS2_STATUS) & 0b10);
}

static inline uint8_t read_data(void) {
    wait_data();

    return inb(PS2_DATA);
}

static inline uint16_t read_data_timeout(void) {
    for (size_t i = 0; i < 0x1000000; i++) {
        if (inb(PS2_STATUS) & 0b1) return inb(PS2_DATA);
    }

    return 0x100;
}

static inline void send_command(uint8_t command) {
    wait_write();
    outb(PS2_COMMAND, command);
}

static inline uint8_t read_command(uint8_t command) {
    send_command(command);
    return read_data();
}

static inline void write_command(uint8_t command, uint8_t data) {
    send_command(command);
    wait_write();
    outb(PS2_DATA, data);
}

static inline uint8_t read_config(void) {
    return read_command(0x20);
}

static inline void write_config(uint8_t config) {
    write_command(0x60, config);
}

static inline int mouse_write(uint8_t command) {
    write_command(0xD4, command);
    
    if (read_data() != 0xFA) return 1;

    return 0;
}

enum {
    MOUSE_BASIC,
    MOUSE_SCROLLING,
    MOUSE_NONE,
} mouse_type;

size_t mouse_packet_loc = 0;
char mouse_packet[4];

static inline bool packet_full(void) {
    if (mouse_type == MOUSE_BASIC && mouse_packet_loc == 3) return true;
    if (mouse_type == MOUSE_SCROLLING && mouse_packet_loc == 4) return true;
    
    return false;
}

void mouse_handler(interrupt_code_t channel, task_state_record_t * tsr, void * interrupt_code) {
    if (mouse_type == MOUSE_NONE || packet_full()) {
        do inb(PS2_DATA);
        while (inb(PS2_STATUS) & 0b1);
        
        return;
    }

    mouse_packet[mouse_packet_loc] = inb(PS2_DATA);

    mouse_packet_loc++;

    if (packet_full()) event_invoke(device->read_ready);
}

void keyboard_handler(interrupt_code_t channel, task_state_record_t * tsr, void * interrupt_code) {
    do inb(PS2_DATA);
    while (inb(PS2_STATUS) & 0b1);
}

uint64_t mouse_read(device_t * dev, char * buffer, uint64_t size) {
    while (!packet_full()) {
        scheduler_await(dev->read_ready);
    }
    
    if (mouse_type == MOUSE_BASIC) {
        if (size != 3) return 0;
        
        memcpy(buffer, mouse_packet, 3);

        mouse_packet_loc = 0;

        return 3;
    }
    else if (mouse_type == MOUSE_SCROLLING) {
        if (size != 4) return 0;
        
        memcpy(buffer, mouse_packet, 4);

        mouse_packet_loc = 0;

        return 4;
    }
    else {
        return 0;
    }
}

int init() {
    if (!interrupt_registry_register((interrupt_code_t) IC_MOUSE, mouse_handler)) return ERROR_INT_UNAVAIL;
    if (!interrupt_registry_register((interrupt_code_t) IC_KEYBOARD, keyboard_handler)) return ERROR_INT_UNAVAIL;

    device_char_operations_t mouse_ops = {
        .write = NULL,
        .read  = mouse_read,
    };
    device_char_data_t data = { };
    device = device_create_char("mouse0", NULL, &mouse_ops, &data);
    devfs_register(device);

    read_data_timeout();
    read_data_timeout();
    read_data_timeout();
    read_data_timeout();

    send_command(0xAD);
    send_command(0xA7);

    while (inb(PS2_STATUS) & 0b1) inb(PS2_DATA);

    if (read_command(0xAA) != 0x55) {
        kprintf("PS2 INIT ERROR: self test failed\n");

        return -1;
    }

    send_command(0xA8);

    uint8_t config = read_config();
    if (config & (1 << 5)) {
        kprintf("PS2 INIT ERROR: dual channel not supported\n");
        
        return -2;
    }

    if (read_command(0xA9) != 0x00) {
        kprintf("PS2 INIT ERROR: port 2 test failed\n");

        return -3;
    }

    send_command(0xAE);
    send_command(0xA8);

    config |=  0b1000011;
    config &= ~0b0110000;

    write_config(config);

    mouse_write(0xF5);
    mouse_write(0xF2);

    uint8_t type_data;

    type_data = read_data();
    read_data_timeout();

    switch (type_data) {
        case 0x00: {
            mouse_type = MOUSE_BASIC;

            kprintf("Mouse type: BASIC");
        } break;

        case 0x03: {
            mouse_type = MOUSE_SCROLLING;

            kprintf("Mouse type: SCROLLING");
        } break;

        default: {
            mouse_type = MOUSE_NONE;

            kprintf("Mouse type: UNKNOWN");
        } break;
    }

    if (mouse_write(0xF4)) kprintf("Couldn't enable data reporting on mouse\n");

    return 0;
}

int free() {
    if (!interrupt_registry_free((interrupt_code_t) IC_KEYBOARD)) return ENOTSUP;

    if (!interrupt_registry_free((interrupt_code_t) IC_MOUSE)) return ENOTSUP;

    return 0;
}

MODULE_NAME("x86_ps2");
MODULE_DEPS("devfs");

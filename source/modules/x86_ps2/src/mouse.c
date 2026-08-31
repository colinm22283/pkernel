#include <interrupt/interrupt_registry.h>

#include <device/device.h>

#include <scheduler/scheduler.h>

#include <devfs/devfs.h>

#include <input/input.h>
#include <input/buttons.h>

#include <util/memory/memcpy.h>

#include <sys/interrupt/interrupt_code.h>
#include <sys/pic/pic.h>

#include <errno.h>

#include <debug/printf.h>

#include <mod_defs.h>

#include <keycode_lut.h>

#define PS2_DATA    (0x60)
#define PS2_STATUS  (0x64)
#define PS2_COMMAND (0x64)

input_device_t * device;

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

static inline void write_data(uint8_t data) {
    wait_write();
    outb(PS2_DATA, data);
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

bool mouse_buttons[3] = { false, false, false, };

static inline bool packet_full(void) {
    if (mouse_type == MOUSE_BASIC && mouse_packet_loc == 3) return true;
    if (mouse_type == MOUSE_SCROLLING && mouse_packet_loc == 4) return true;
    
    return false;
}

void mouse_handler(interrupt_code_t channel, task_state_record_t * tsr, void * interrupt_code) {
    if (mouse_type == MOUSE_NONE) {
        do inb(PS2_DATA);
        while (inb(PS2_STATUS) & 0b1);
        
        return;
    }

    char data = inb(PS2_DATA);
    if (mouse_packet_loc == 0 && !(data & (1 << 3))) return;

    mouse_packet[mouse_packet_loc] = data;

    mouse_packet_loc++;

    if (packet_full()) {
        bool buttons[3] = {
            !!(mouse_packet[0] & (1 << 0)),
            !!(mouse_packet[0] & (1 << 1)),
            !!(mouse_packet[0] & (1 << 2)),
        };

        for (int i = 0; i < 3; i++) {
            if (buttons[i] ^ mouse_buttons[i]) {
                char buffer[sizeof(input_packet_header_t) + sizeof(input_packet_key_t)];
                input_packet_t * packet = (input_packet_t *) &buffer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
                packet->header.type = IT_KEY;
                packet->header.size = sizeof(input_packet_header_t) + sizeof(input_packet_key_t);
                packet->data.key.action = buttons[i] ? IT_ACTION_PRESS : IT_ACTION_RELEASE;
                packet->data.key.code   = BTN_LEFT + i;

                input_provide_packet(device, packet);
#pragma GCC diagnostic pop
            }
        }

        if (mouse_packet[1] != 0) {
            char buffer[sizeof(input_packet_header_t) + sizeof(input_packet_rel_t)];
            input_packet_t * packet = (input_packet_t *) &buffer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
            packet->header.type = IT_REL;
            packet->header.size = sizeof(input_packet_header_t) + sizeof(input_packet_rel_t);
            packet->data.rel.axis  = IT_REL_X;
            packet->data.rel.value = mouse_packet[1];

            input_provide_packet(device, packet);
#pragma GCC diagnostic pop
        }

        if (mouse_packet[2] != 0) {
            char buffer[sizeof(input_packet_header_t) + sizeof(input_packet_rel_t)];
            input_packet_t * packet = (input_packet_t *) &buffer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
            packet->header.type = IT_REL;
            packet->header.size = sizeof(input_packet_header_t) + sizeof(input_packet_rel_t);
            packet->data.rel.axis  = IT_REL_Y;
            packet->data.rel.value = mouse_packet[2];

            input_provide_packet(device, packet);
#pragma GCC diagnostic pop
        }

        mouse_packet_loc = 0;

        for (int i = 0; i < 3; i++) {
            mouse_buttons[i] = buttons[i];
        }
    }
}

bool awaiting_extra_keycode = false;
bool key_release = false;

void keyboard_handler(interrupt_code_t channel, task_state_record_t * tsr, void * interrupt_code) {
    uint8_t data = inb(PS2_DATA);

    if (data == 0xE0) {
        awaiting_extra_keycode = true;
    }
    else if (data == 0xF0) {
        key_release = true;
    }
    else {
        uint16_t keycode;

        if (awaiting_extra_keycode) {
            keycode = keycode_lut[data + 256];
        }
        else {
            keycode = keycode_lut[data];
        }

        char buffer[sizeof(input_packet_header_t) + sizeof(input_packet_key_t)];
        input_packet_t * packet = (input_packet_t *) &buffer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
        packet->header.type = IT_KEY;
        packet->header.size = sizeof(input_packet_header_t) + sizeof(input_packet_key_t);
        packet->data.key.action = key_release ? IT_ACTION_RELEASE : IT_ACTION_PRESS;
        packet->data.key.code   = keycode;

        input_provide_packet(device, packet);
#pragma GCC diagnostic pop

        awaiting_extra_keycode = false;
        key_release = false;
    }
}

int init() {
    if (!interrupt_registry_register((interrupt_code_t) IC_MOUSE, mouse_handler)) return ERROR_INT_UNAVAIL;
    if (!interrupt_registry_register((interrupt_code_t) IC_KEYBOARD, keyboard_handler)) return ERROR_INT_UNAVAIL;

    device = input_add_device(NULL, NULL);

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

    config |=  0b0000011;
    config &= ~0b1110000;

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

    read_data_timeout();
    read_data_timeout();
    read_data_timeout();
    mouse_packet_loc = 0;

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

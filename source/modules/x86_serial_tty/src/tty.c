#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <device/device.h>
#include <devfs/devfs.h>

#include <interrupt/interrupt_registry.h>

#include <scheduler/scheduler.h>

#include <mod_defs.h>

#include <sys/port.h>

#include <sys/interrupt/interrupt_code.h>

#include <sys/asm/out.h>
#include <sys/asm/in.h>

#include <debug/vga_print.h>

typedef struct {
    port_t port;
    event_t * event;
    bool * waiting;
    uint8_t * data;
} tty_private_t;

tty_private_t private[2];
device_t * devices[2];
devfs_entry_t * devfs_entries[2];

event_t * read_events[2];

bool char_waiting[2];
uint8_t port_data[2];

static inline uint8_t read_com1(void) { return inb(private[0].port); }
static inline uint8_t read_com2(void) { return inb(private[1].port); }

void com1_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    char_waiting[0] = true;
    port_data[0] = read_com1();

    event_invoke(read_events[0]);
}

void com2_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    char_waiting[1] = true;
    port_data[1] = read_com2();

    event_invoke(read_events[1]);
}

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    tty_private_t * private = dev->private;

    for (uint64_t i = 0; i < size; i++) {
        outb(private->port, buffer[i]);
    }

    return size;
}

uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    tty_private_t * private = dev->private;

    if (!*private->waiting) {
        scheduler_await(private->event);
    }

    buffer[0] = (char) *private->data;

    *private->waiting = false;

    return 1;
}

bool init(void) {
    device_char_operations_t operations = {
        .write = write,
        .read = read,
    };
    device_char_data_t data = { };

    read_events[0] = event_init();
    read_events[1] = event_init();

    private[0].port = 0x3F8;
    private[0].event = read_events[0];
    private[0].waiting = &char_waiting[0];
    private[0].data = &port_data[0];

    private[1].port = 0x2F8;
    private[1].event = read_events[1];
    private[1].waiting = &char_waiting[1];
    private[1].data = &port_data[1];

    char_waiting[0] = false;
    char_waiting[1] = false;

    for (size_t i = 0; i < 10; i++) {
        read_com1();
        read_com2();
    }

    outb(private[0].port + 1, 1);
    outb(private[1].port + 1, 1);

    devices[0] = device_create_char("tty0", &private[0], &operations, &data);
    devfs_entries[0] = devfs_register(devices[0]);

    devices[1] = device_create_char("tty1", &private[1], &operations, &data);
    devfs_entries[1] = devfs_register(devices[1]);

    interrupt_registry_register((interrupt_code_t) IC_COM1, com1_int_handler);
    interrupt_registry_register((interrupt_code_t) IC_COM2, com2_int_handler);

    return true;
}

bool free(void) {
    return true;
}


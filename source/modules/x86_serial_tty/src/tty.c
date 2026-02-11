#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <device/device.h>
#include <devfs/devfs.h>

#include <interrupt/interrupt_registry.h>

#include <scheduler/scheduler.h>

#include <sys/port.h>

#include <sys/interrupt/interrupt_code.h>

#include <sys/asm/out.h>
#include <sys/asm/in.h>

#include <mod_defs.h>

typedef struct {
    port_t port;
    
    device_t * device;
    devfs_entry_t * devfs_entry;

    event_t * data_ready;

    bool char_waiting;
    uint8_t port_data;
} port_data_t;

#ifdef COM1_ENABLE

port_data_t com1_data;

static inline uint8_t read_com1(void) { return inb(com1_data.port); }

void com1_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    debug_print("COM1\n");

    com1_data.char_waiting = true;
    com1_data.port_data = read_com1();

    event_invoke(com1_data.data_ready);
}

#endif

#ifdef COM2_ENABLE

port_data_t com2_data;

static inline uint8_t read_com2(void) { return inb(com2_data.port); }

void com2_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    debug_print("COM2\n");

    com2_data.char_waiting = true;
    com2_data.port_data = read_com2();

    event_invoke(com2_data.data_ready);
}

#endif

uint64_t write(device_t * dev, const char * buffer, uint64_t size) {
    port_data_t * private = dev->private;

    for (uint64_t i = 0; i < size; i++) {
        while ((inb(private->port + 5) & 0x20) == 0) { }

        outb(private->port, buffer[i]);
    }

    return size;
}

uint64_t read(device_t * dev, char * buffer, uint64_t size) {
    port_data_t * private = dev->private;

    if (!private->char_waiting) {
        scheduler_await(private->data_ready);
    }

    if (private->port_data == '\r') {
        buffer[0] = '\n';
    }
    else {
        buffer[0] = (char) private->port_data;
    }

    private->char_waiting = false;

    return 1;
}

error_number_t init(void) {
    device_char_operations_t operations = {
        .write = write,
        .read = read,
    };
    device_char_data_t data = { };

#ifdef COM1_ENABLE

    MODULE_DEBUG(
        MODULE_PRINT("Initializing COM1\n");
    );
    
    // com1_data.port = 0x3F8;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    com1_data.port = *(uint16_t *) 0x400;
#pragma GCC diagnostic pop

    com1_data.data_ready = event_init();
    com1_data.char_waiting = false;
    com1_data.port_data = 0;
    for (size_t i = 0; i < 10; i++) read_com1();

    com1_data.device = device_create_char("tty0", &com1_data, &operations, &data);
    com1_data.devfs_entry = devfs_register(com1_data.device);

    if (!interrupt_registry_register((interrupt_code_t) IC_COM1, com1_int_handler)) return ERROR_INT_UNAVAIL;

    outb(com1_data.port + 3, 0b10000000);

    outb(com1_data.port + 0, 1);
    outb(com1_data.port + 1, 0);

    outb(com1_data.port + 3, 0b00000011);

    outb(com1_data.port + 1, 0x01);

    outb(com1_data.port + 2, 0b110);

    MODULE_DEBUG(
        MODULE_PRINT("COM1 Initialized\n");
    );
    
#endif

#ifdef COM2_ENABLE

    MODULE_DEBUG(
        MODULE_PRINT("Initializing COM2\n");
    );
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    com2_data.port = *(uint16_t *) 0x402;
#pragma GCC diagnostic pop
    com2_data.data_ready = event_init();
    com2_data.char_waiting = false;
    com2_data.port_data = 0;
    for (size_t i = 0; i < 10; i++) read_com2();

    com2_data.device = device_create_char("tty1", &com2_data, &operations, &data);
    com2_data.devfs_entry = devfs_register(com2_data.device);

    if (!interrupt_registry_register((interrupt_code_t) IC_COM2, com2_int_handler)) return ERROR_INT_UNAVAIL;

    outb(com2_data.port + 3, 0b10000000);

    outb(com2_data.port + 0, 1);
    outb(com2_data.port + 1, 0);

    outb(com2_data.port + 3, 0b00000011);

    outb(com2_data.port + 1, 0x01);

    outb(com2_data.port + 2, 0b110);

    MODULE_DEBUG(
        MODULE_PRINT("COM2 Initialized\n");
    );
    
#endif

    return ERROR_OK;
}

error_number_t free(void) {
    return ERROR_OK;
}

MODULE_NAME("x86_serial_tty");
MODULE_DEPS("devfs", "sysfs");

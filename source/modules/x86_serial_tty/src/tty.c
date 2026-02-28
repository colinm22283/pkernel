#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <device/device.h>
#include <devfs/devfs.h>

#include <interrupt/interrupt_registry.h>

#include <scheduler/scheduler.h>

#include <tty/tty.h>

#include <sys/port.h>

#include <sys/interrupt/interrupt_code.h>

#include <sys/asm/out.h>
#include <sys/asm/in.h>

#include <mod_defs.h>

typedef struct {
    port_t port;
    
    tty_t * tty;
} port_data_t;

#ifdef COM1_ENABLE

port_data_t com1_data;

static inline uint8_t read_com1(void) { return inb(com1_data.port); }

void com1_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    char c = read_com1();

    if (c == 0x7F) {
        tty_provide_char(com1_data.tty, 0x08);
    }
    else {
        tty_provide_char(com1_data.tty, c);
    }
}

#endif

#ifdef COM2_ENABLE

port_data_t com2_data;

static inline uint8_t read_com2(void) { return inb(com2_data.port); }

void com2_int_handler(interrupt_code_t channel, task_state_record_t * isr, void * interrupt_code) {
    char c = read_com2();

    if (c == 0x7F) {
        tty_provide_char(com2_data.tty, 0x08);
    }
    else {
        tty_provide_char(com2_data.tty, c);
    }
}

#endif

uint64_t write(tty_t * tty, const char * buffer, uint64_t size) {
    port_data_t * private = tty->cookie;

    for (uint64_t i = 0; i < size; i++) {
        while ((inb(private->port + 5) & 0x20) == 0) { }

        outb(private->port, buffer[i]);
    }

    return size;
}

int init(void) {
#ifdef COM1_ENABLE

    MODULE_DEBUG(
        MODULE_PRINT("Initializing COM1");
    );
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    com1_data.port = *(uint16_t *) 0x400;
#pragma GCC diagnostic pop

    if (com1_data.port == 0) com1_data.port = 0x3F8;

    MODULE_DEBUG(
        MODULE_PRINT("Com1 port: ");
        MODULE_PRINT_HEX(com1_data.port);
    );

    com1_data.tty = tty_init(write, &com1_data);

    for (size_t i = 0; i < 10; i++) read_com1();

    if (!interrupt_registry_register((interrupt_code_t) IC_COM1, com1_int_handler)) return ERROR_INT_UNAVAIL;

    outb(com1_data.port + 3, 0b10000000);

    outb(com1_data.port + 0, 1);
    outb(com1_data.port + 1, 0);

    outb(com1_data.port + 3, 0b00000011);

    outb(com1_data.port + 1, 0x01);

    outb(com1_data.port + 2, 0b110);

    outb(com1_data.port + 4, 0x0F);
    
#endif

#ifdef COM2_ENABLE

    MODULE_DEBUG(
        MODULE_PRINT("Initializing COM2");
    );
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    com2_data.port = *(uint16_t *) 0x402;
#pragma GCC diagnostic pop

    if (com2_data.port == 0) com2_data.port = 0x2F8;

    MODULE_DEBUG(
        MODULE_PRINT("Com2 port: ");
        MODULE_PRINT_HEX(com2_data.port);
    );

    com2_data.tty = tty_init(write, &com2_data);

    for (size_t i = 0; i < 10; i++) read_com2();

    if (!interrupt_registry_register((interrupt_code_t) IC_COM2, com2_int_handler)) return ERROR_INT_UNAVAIL;

    outb(com2_data.port + 3, 0b10000000);

    outb(com2_data.port + 0, 1);
    outb(com2_data.port + 1, 0);

    outb(com2_data.port + 3, 0b00000011);

    outb(com2_data.port + 1, 0x01);

    outb(com2_data.port + 2, 0b110);

    outb(com2_data.port + 4, 0x0F);
    
#endif

    return ERROR_OK;
}

int free(void) {
#ifdef COM1_ENABLE
    interrupt_registry_free((interrupt_code_t) IC_COM1);

    tty_free(com1_data.tty);
#endif

#ifdef COM2_ENABLE
    interrupt_registry_free((interrupt_code_t) IC_COM2);

    tty_free(com2_data.tty);
#endif

    return ERROR_OK;
}

MODULE_NAME("x86_serial_tty");
MODULE_DEPS("devfs", "sysfs");

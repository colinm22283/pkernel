#include <interrupt/pic.h>
#include <interrupt/idt.h>
#include <interrupt/handlers_entries.h>

#include <memory/gdt.h>

#include <sys/ports.h>
#include <sys/asm/out.h>
#include <sys/asm/in.h>
#include <sys/asm/io_wait.h>

#define ICW1_ICW4       (0x01)
#define ICW1_SINGLE     (0x02)
#define ICW1_INTERVAL4  (0x04)
#define ICW1_LEVEL      (0x08)
#define ICW1_INIT       (0x10)

#define ICW4_8086       (0x01)
#define ICW4_AUTO       (0x02)
#define ICW4_BUF_SLAVE  (0x08)
#define ICW4_BUF_MASTER (0x0C)
#define ICW4_SFNM       (0x10)

void interrupt_pic_init(void) {
    interrupt_pic_remap(0x20, 0x28);

    idt.mapped_irqs[MAPPED_IRQ_TIMER] = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, pic1_timer_handler_entry);
    idt.mapped_irqs[MAPPED_IRQ_KEYBOARD] = DEFINE_IDT64_ENTRY_INTERRUPT(GDT_KERNEL_CODE, pic1_keyboard_handler_entry);

    interrupt_pic_clear_mask(MAPPED_IRQ_TIMER);
    interrupt_pic_clear_mask(MAPPED_IRQ_KEYBOARD);

    inb(0x60); // clear out the keyboard buffer

    outb(PORT_PIC1_COMMAND, 0x20);
    outb(PORT_PIC2_COMMAND, 0x20);
}

void interrupt_pic_remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t master_mask, slave_mask;

    master_mask = inb(PORT_PIC1_DATA);
    slave_mask  = inb(PORT_PIC2_DATA);

    outb(PORT_PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PORT_PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PORT_PIC1_DATA, master_offset);
    io_wait();
    outb(PORT_PIC2_DATA, slave_offset);
    io_wait();
    outb(PORT_PIC1_DATA, 4);
    io_wait();
    outb(PORT_PIC2_DATA, 2);
    io_wait();

    outb(PORT_PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PORT_PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PORT_PIC1_DATA, master_mask);
    outb(PORT_PIC2_DATA, slave_mask);
}

void interrupt_pic_mask_all(void) {
    outb(PORT_PIC1_DATA, 0xFF);
    outb(PORT_PIC2_DATA, 0xFF);
}

void interrupt_pic_set_mask(unsigned char irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PORT_PIC1_DATA;
    } else {
        port = PORT_PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) | (1 << irq_line);
    outb(port, value);
}

void interrupt_pic_clear_mask(unsigned char irq_line) {
    uint16_t port;
    uint8_t value;

    if(irq_line < 8) {
        port = PORT_PIC1_DATA;
    } else {
        port = PORT_PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}
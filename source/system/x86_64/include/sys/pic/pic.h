#pragma once

#include <stdint.h>

enum {
    MAPPED_IRQ_TIMER = 0,
    MAPPED_IRQ_KEYBOARD = 1,
};

void interrupt_pic_init(void);

void interrupt_pic_remap(uint8_t master_offset, uint8_t slave_offset);

void interrupt_pic_mask_all(void);
void interrupt_pic_set_mask(unsigned char irq_line);
void interrupt_pic_clear_mask(unsigned char irq_line);
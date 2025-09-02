#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint16_t size;
    void * offset;
} idt_ptr_t;

#define DEFINE_IDT_POINTER(idt) ((idt_ptr_t) { .size = sizeof((idt)) - 1, .offset = &(idt), })
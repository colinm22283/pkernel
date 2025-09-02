#pragma once

#include <stdint.h>

#include <defs.h>

#include <sys/gdt/gdt_flags.h>

typedef struct __PACKED {
    uint16_t limit_lower : 16;
    uint32_t base_lower  : 24;
    uint8_t access       :  8;
    uint8_t limit_upper  :  4;
    uint8_t flags        :  4;
    uint64_t base_upper  : 40;
    uint32_t reserved    : 32;
} gdt64_entry_t;

typedef struct __PACKED {
    uint16_t size : 16;
    union {
        void * pointer;
        uint64_t int_pointer;
    };
} gdt64_ptr_t;

#define GDT64_NULL_ENTRY ((gdt64_entry_t) { .limit_lower = 0, .access = 0, .flags = 0, .base_upper = 0, .base_lower = 0, .limit_upper = 0, })
#define DEFINE_GDT64_POINTER(gdt_table) ((gdt64_ptr_t) { .size = sizeof(gdt_table) - 1, .pointer = &(gdt_table), })
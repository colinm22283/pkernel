#pragma once

#include <sys/gdt/gdt64.h>

#define GDT_KERNEL_CODE (0x10)
#define GDT_KERNEL_DATA (0x20)
#define GDT_KERNEL_TSS  (0x30)
#define GDT_USER_CODE   (0x40)
#define GDT_USER_DATA   (0x50)

typedef struct __PACKED {
    gdt64_entry_t null;
    gdt64_entry_t kernel_code;
    gdt64_entry_t kernel_data;

    gdt64_entry_t kernel_tss;

    gdt64_entry_t user_code;
    gdt64_entry_t user_data;
} gdt64_t;

extern gdt64_ptr_t gdt_ptr;

extern gdt64_t gdt;

void gdt_init(void);

#pragma once

#include <task/tss.h>

#include <sys/gdt/gdt64.h>

#include <sys/asm/lgdt.h>
#include <sys/asm/ltr.h>

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

extern gdt64_t gdt;

extern gdt64_ptr_t gdt_ptr;

static inline void gdt_init(void) {
    uint64_t tss_base = (uint64_t) &tss;
    uint32_t tss_limit = sizeof(tss64_t);

    gdt.kernel_tss.base_upper = tss_base >> 24;
    gdt.kernel_tss.base_lower = tss_base;

    gdt.kernel_tss.limit_upper = tss_limit >> 16;
    gdt.kernel_tss.limit_lower = tss_limit;

    gdt.kernel_tss.access =
        GDT_TSS_ACCESS_TSS64 |
        GDT_TSS_ACCESS_PRESENT |
        GDT_TSS_ACCESS_PRIVILEGE_LEVEL(0);

    gdt.kernel_tss.flags = GDT_FLAGS_SIZE;

    tss.rsp0 = (uint64_t) STACK_TOP;
    tss.ist1 = (uint64_t) STACK_TOP;

    lgdt(&gdt_ptr);

    ltr(GDT_KERNEL_TSS);
}
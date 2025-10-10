#include <sys/gdt/gdt.h>

#include <sys/tss/tss.h>

#include <sys/asm/lgdt.h>
#include <sys/asm/ltr.h>

gdt64_t gdt = {
    .null = GDT64_NULL_ENTRY,
    .kernel_code = {
        .base_upper = 0,
        .base_lower = 0,
        .limit_upper = 0xF,
        .limit_lower = 0xFFFF,
        .flags =
            GDT_FLAGS_GRANULARITY |
            GDT_FLAGS_LONG_MODE,
        .access =
            GDT_ACCESS_PRESENT |
            GDT_ACCESS_PRIVILEGE_LEVEL(0) |
            GDT_ACCESS_TYPE |
            GDT_ACCESS_EXECUTABLE |
            GDT_ACCESS_READ_WRITE |
            GDT_ACCESS_ACCESSED,
    },
    .kernel_data = {
        .base_upper = 0,
        .base_lower = 0,
        .limit_upper = 0xF,
        .limit_lower = 0xFFFF,
        .flags =
            GDT_FLAGS_GRANULARITY |
            GDT_FLAGS_LONG_MODE,
        .access =
            GDT_ACCESS_PRESENT |
            GDT_ACCESS_PRIVILEGE_LEVEL(0) |
            GDT_ACCESS_TYPE |
            GDT_ACCESS_READ_WRITE |
            GDT_ACCESS_ACCESSED,
    },

    .user_data = {
        .base_upper = 0,
        .base_lower = 0,
        .limit_upper = 0xF,
        .limit_lower = 0xFFFF,
        .flags =
        GDT_FLAGS_GRANULARITY |
        GDT_FLAGS_LONG_MODE,
        .access =
        GDT_ACCESS_PRESENT |
        GDT_ACCESS_PRIVILEGE_LEVEL(3) |
        GDT_ACCESS_TYPE |
        GDT_ACCESS_READ_WRITE |
        GDT_ACCESS_ACCESSED,
    },
    .user_code = {
        .base_upper = 0,
        .base_lower = 0,
        .limit_upper = 0xF,
        .limit_lower = 0xFFFF,
        .flags =
            GDT_FLAGS_GRANULARITY |
            GDT_FLAGS_LONG_MODE,
        .access =
            GDT_ACCESS_PRESENT |
            GDT_ACCESS_PRIVILEGE_LEVEL(3) |
            GDT_ACCESS_TYPE |
            GDT_ACCESS_EXECUTABLE |
            GDT_ACCESS_READ_WRITE |
            GDT_ACCESS_ACCESSED,
    },
};

gdt64_ptr_t gdt_ptr = DEFINE_GDT64_POINTER(gdt);

void gdt_init(void) {
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

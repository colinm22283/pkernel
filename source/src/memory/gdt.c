#include <memory/gdt.h>

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
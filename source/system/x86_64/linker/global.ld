SECTIONS {
    .memory_map 0x500 (NOLOAD) : {
        *(.memory_map);
    } > usable_memory_2

    .memory_map_length 0x500 + 0x400 (NOLOAD) : {
        *(.memory_map_size);
    } > usable_memory_2
    .kernel_region 0x500 + 0x400 + 0x4 (NOLOAD) : {
        *(.kernel_region_base);
        *(.kernel_region_length);
    } > usable_memory_2
}
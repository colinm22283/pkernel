MEMORY {
    ivt                  : ORIGIN = 0x000000, LENGTH = 0x000400 - 0x000000
    bda                  : ORIGIN = 0x000400, LENGTH = 0x000500 - 0x000400
    usable_memory_1      : ORIGIN = 0x000500, LENGTH = 0x007C00 - 0x000500
    boot_sector          : ORIGIN = 0x007C00, LENGTH = 0x007E00 - 0x007C00
    usable_memory_2      : ORIGIN = 0x007E00, LENGTH = 0x080000 - 0x007E00
    ebda                 : ORIGIN = 0x080000, LENGTH = 0x0A0000 - 0x080000
    video_display_memory : ORIGIN = 0x0A0000, LENGTH = 0x0C0000 - 0x0A0000
    video_bios           : ORIGIN = 0x0C0000, LENGTH = 0x0C8000 - 0x0C0000
    bios_expansions      : ORIGIN = 0x0C8000, LENGTH = 0x0F0000 - 0x0C8000
    bios                 : ORIGIN = 0x0F0000, LENGTH = 0x100000 - 0x0F0000

    kernel_memory        : ORIGIN = 0x100000, LENGTH = 0x10000000000000000 - 0x100000
}
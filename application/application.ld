OUTPUT_FORMAT("elf64-x86-64");
ENTRY(_start);
SECTIONS {
    . = 0x8100000000;
    .text . : {
        *(.start);
        *(.text*);
    }

    . = 0x8200000000;
    .data . : {
        *(.data*);
    }

    . = 0x8300000000;
    .rodata . : {
        *(.rodata*);
    }
    . = 0x8400000000;
    .bss . (NOLOAD) : {
        *(.bss);
    }
}

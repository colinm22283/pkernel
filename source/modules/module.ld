OUTPUT_FORMAT("binary");
ENTRY(init);
SECTIONS {
    . = 0;
    .module . : AT(0) {
        _entry_table_start = .;
        *(.entry_table);
        _entry_table_end = .;
        *(.text*);
    }
    .data . : {
        *(.rodata*);
        *(.data*);
    }
    .bss . (NOLOAD) : {
        *(.bss*);
    }

    ASSERT(_entry_table_start != _entry_table_end, "Module must contain an entry table")
}
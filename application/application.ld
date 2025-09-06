OUTPUT_FORMAT("binary");
ENTRY(_start);
SECTIONS {
    .start_table 0 : AT(0x00) {
        _start_table_start = .;
        KEEP(*(.start_table));
        _start_table_end = .;

        _start_table_size = _start_table_end - _start_table_start;
    }


    . = 0x8100000000;
    _text_start = .;
    .text . : AT(_start_table_size) {
        *(.start);
        *(.text*);
    }
    _text_end = .;
    _text_size = _text_end - _text_start;

    . = 0x8200000000;
    _data_start = .;
    .data . : AT(_start_table_size + _text_size) {
        *(.data*);
    }
    _data_end = .;
    _data_size = _data_end - _data_start;

    . = 0x8300000000;
    _rodata_start = .;
    .rodata . : AT(_start_table_size + _text_size + _data_size) {
        *(.rodata*);
    }
    _rodata_end = .;
    _rodata_size = _rodata_end - _rodata_start;

    . = 0x8400000000;
    _bss_start = .;
    .bss . (NOLOAD) : {
        *(.bss);
    }
    _bss_end = .;
    _bss_size = _bss_end - _bss_start;
}

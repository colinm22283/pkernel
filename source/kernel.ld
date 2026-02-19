ENTRY(kernel_entry);
SECTIONS {
    . = 0xC0000000;

    _kernel_start = .;

    _text_start = .;
    .kernel_entry . : AT(0) {
        *(.kernel_entry);
    } > kernel_memory

    .text . : {
        *(.text*);
    } > kernel_memory
    _text_end = .;

    _text_size = _text_end - _text_start;

    . = ALIGN(0x1000);
    _data_start = .;

    .data . : {
        *(.rodata*);
        *(.data*);
    } > kernel_memory

    .bss . (NOLOAD) : {
        *(.bss*);
        *(.lbss*);
    } > kernel_memory

    .stack . (NOLOAD) : {
        _stack_base = .;
        . += 4096;
        . = ALIGN(8);
        _stack_top = .;

        _stack_size = _stack_top - _stack_base;
    } > kernel_memory

    _data_end = .;
    _data_size = _data_end - _data_start;
    . = ALIGN(0x1000);

    .modules . : {
        _modules_text = .;
        *(.module_export);
        *(.module_text);
        . = ALIGN(0x1000);
        _modules_data = .;
        *(.module_data);
        *(.module_rodata);
        *(.module_bss);
        _modules_end = .;
    } > kernel_memory

    _kernel_end = .;

    _kernel_size = _kernel_end - _kernel_start;

    /DISCARD/ : {
        *(.entry_table);
    }
}

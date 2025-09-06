#include <application/application_start_table.h>

#include <defs.h>

extern char _text_size;

extern char _data_size;

extern char _rodata_size;

extern char _bss_size;

__SECTION(".start_table") application_start_table_t _application_start_table = {
    .text_size = (uint64_t) &_text_size,
    .data_size = (uint64_t) &_data_size,
    .rodata_size = (uint64_t) &_rodata_size,
    .bss_size = (uint64_t) &_bss_size,
};
#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint8_t present           :  1;
    uint8_t write             :  1;
    uint8_t user              :  1;
    uint8_t reserved_write    :  1;
    uint8_t instruction_fetch :  1;
    uint8_t protection_key    :  1;
    uint8_t shadow_stack      :  1;
    uint8_t _reserved0        :  8;
    uint8_t sgx               :  1;
    uint16_t _reserved1       : 16;
} page_fault_error_code_t;
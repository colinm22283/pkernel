#pragma once

#include <stdint.h>

#include <defs.h>

typedef struct __PACKED {
    uint16_t offset_lower : 16;
    uint16_t selector     : 16;
    uint8_t  ist          :  3;
    uint8_t  _reserved0   :  5;
    uint8_t  gate_type    :  4;
    uint8_t  _reserved1   :  1;
    uint8_t  dpl          :  2;
    uint8_t  p            :  1;
    uint64_t offset_upper : 48;
    uint32_t _reserved2   : 32;
} idt64_entry_t;

typedef struct __PACKED {
    idt64_entry_t
        div0,                     //  0
        _res0,                    //  1
        nmi,                      //  2
        bp_int3,                  //  3
        ovf,                      //  4
        bound_range,              //  5
        invalid_opcode,           //  6
        device_not_avail,         //  7
        double_fault,             //  8
        coproc_segment_overrun,   //  9
        invalid_tss,              //  A
        segment_not_present,      //  B
        stack_segment_fault,      //  C
        general_protection_fault, //  D
        page_fault,               //  E
        _res1,                    //  F
        x87_fpu_error,            // 10
        alignment_check,          // 11
        machine_check,            // 12
        simd_fp_error,            // 13
        _res2;                    // 14

    idt64_entry_t _undefined_entries0[11];

    idt64_entry_t mapped_irqs[16];

    idt64_entry_t system_interrupt;

    idt64_entry_t _undefined_entries1[207];
} idt64_t;

#define DEFINE_IDT64_ENTRY_INTERRUPT(_selector, handler) ((idt64_entry_t) { \
    .offset_lower = (uint16_t) (((uint64_t) (intptr_t) &handler) & 0xFFFF), \
    .offset_upper = ((uint64_t) (intptr_t) &handler) >> 16,                 \
    .ist = 1,                                                    \
    .dpl = 0,                                                    \
    .p = 1,                                                      \
    .gate_type = 0xE,                                            \
    .selector = _selector,                                        \
    ._reserved0 = 0,                                             \
    ._reserved1 = 0,                                             \
    ._reserved2 = 0,                                             \
})

#define DEFINE_IDT64_ENTRY_USER_INTERRUPT(_selector, handler) ((idt64_entry_t) { \
    .offset_lower = (uint16_t) (((uint64_t) &handler) & 0xFFFF), \
    .offset_upper = ((uint64_t) &handler) >> 16,                 \
    .ist = 1,                                                    \
    .dpl = 3,                                                    \
    .p = 1,                                                      \
    .gate_type = 0xE,                                            \
    .selector = _selector,                                        \
    ._reserved0 = 0,                                             \
    ._reserved1 = 0,                                             \
    ._reserved2 = 0,                                             \
})

#define DEFINE_IDT64_ENTRY_NULL ((idt64_entry_t) { \
    .offset_lower = 0, \
    .offset_upper = 0, \
    .ist = 0,          \
    .dpl = 0,          \
    .p = 0,            \
    .gate_type = 0,    \
    .selector = 0,     \
    ._reserved0 = 0,   \
    ._reserved1 = 0,   \
    ._reserved2 = 0,   \
})

#pragma once

#include <stdint.h>

#include "../../defs.h"

typedef struct __PACKED {
    uint32_t _reserved0 : 32;
    uint64_t rsp0       : 64;
    uint64_t rsp1       : 64;
    uint64_t rsp2       : 64;
    uint64_t _reserved1 : 64;
    uint64_t ist1       : 64;
    uint64_t ist2       : 64;
    uint64_t ist3       : 64;
    uint64_t ist4       : 64;
    uint64_t ist5       : 64;
    uint64_t ist6       : 64;
    uint64_t ist7       : 64;
    uint64_t _reserved2 : 64;
    uint32_t iopb       : 32;
} tss64_t;
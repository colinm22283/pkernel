#pragma once

#include <defs.h>

typedef uint32_t msr_number_t;

#define MSR_IA32_EFER ((msr_number_t) 0xC0000080)
#define MSR_IA32_APIC ((msr_number_t) 0x1B)
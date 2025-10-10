#pragma once

#include <stdint.h>

#include <sys/tsr/tsr.h>

#include <defs.h>

__NORETURN void enter_user_mode(uint64_t cs_selector, uint64_t ss_selector, uint64_t pml4t_paddr, interrupt_state_record_t * isr);
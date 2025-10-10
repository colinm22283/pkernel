#pragma once

#include <paging/tables.h>
#include <paging/kernel_translation.h>

#include <memory/gdt.h>

#include <sys/isr/isr.h>

#include <defs.h>

__NORETURN void _resume_isr_kernel(uint64_t cs_selector, uint64_t ss_selector, uint64_t pml4t_paddr, interrupt_state_record_t * isr);

__NORETURN static inline void resume_isr_kernel(interrupt_state_record_t * isr) {
    _resume_isr_kernel(GDT_KERNEL_CODE, GDT_KERNEL_DATA, paging_kernel_virtual_to_physical(&paging_kernel_pml4t), isr);
}

__NORETURN static inline void resume_isr_user(interrupt_state_record_t * isr) {

}
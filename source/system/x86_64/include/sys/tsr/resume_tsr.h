#pragma once

#include <paging/tables.h>
#include <paging/kernel_translation.h>
#include <paging/manager.h>

#include <sys/gdt/gdt.h>

#include <sys/tsr/tsr.h>

#include <defs.h>

__NORETURN void _resume_tsr_kernel(uint64_t cs_selector, uint64_t ss_selector, uint64_t pml4t_paddr, task_state_record_t * tsr);
__NORETURN void _resume_tsr_user(uint64_t cs_selector, uint64_t ss_selector, uint64_t pml4t_paddr, task_state_record_t * tsr);

__NORETURN static inline void resume_tsr_kernel(task_state_record_t * tsr) {
    _resume_tsr_kernel(GDT_KERNEL_CODE, GDT_KERNEL_DATA, paging_kernel_virtual_to_physical(&paging_kernel_pml4t), tsr);
}

__NORETURN static inline void resume_tsr_user(task_state_record_t * tsr, pman_context_t * paging_context) {
    static task_state_record_t static_tsr;
    static_tsr = *tsr;

    _resume_tsr_user(GDT_USER_CODE, GDT_USER_DATA, paging_context->top_level_table_paddr, &static_tsr);
}
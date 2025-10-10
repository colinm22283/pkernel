#include <_process/scheduler.h>

#include <paging/kernel_translation.h>

#include <interrupt/interrupt_registry.h>

#include <sys/isr/isr.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <sys/interrupt/interrupt_code.h>

#include <defs.h>

__NORETURN void div0_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_DIV0, isr, NULL);

    scheduler_start();
}

__NORETURN void nmi_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_NMI, isr, NULL);

    scheduler_start();
}

__NORETURN void bp_int3_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_BP_INT3, isr, NULL);

    scheduler_start();
}

__NORETURN void ovf_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_OVF, isr, NULL);

    scheduler_start();
}

__NORETURN void bound_range_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_BOUND_RANGE, isr, NULL);

    scheduler_start();
}

__NORETURN void invalid_opcode_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_INVALID_OPCODE, isr, NULL);

    scheduler_start();
}

__NORETURN void device_not_avail_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_DEVICE_NOT_AVAIL, isr, NULL);

    scheduler_start();
}

__NORETURN void double_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_DOUBLE_FAULT, isr, NULL);

    scheduler_start();
}

__NORETURN void coproc_segment_overrun_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_COPROC_SEGMENT_OVERRUN, isr, NULL);

    scheduler_start();
}

__NORETURN void invalid_tss_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_INVALID_TSS, isr, NULL);

    scheduler_start();
}

__NORETURN void segment_not_present_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_SEGMENT_NOT_PRESENT, isr, NULL);

    scheduler_start();
}

__NORETURN void stack_segment_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_STACK_SEGMENT_FAULT, isr, NULL);

    scheduler_start();
}

__NORETURN void general_protection_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_GENERAL_PROTECTION_FAULT, isr, NULL);

    scheduler_start();
}

__NORETURN void page_fault_handler(interrupt_state_record_t * isr, page_fault_error_code_t * error_code) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_PAGE_FAULT, isr, error_code);

    scheduler_start();
}

__NORETURN void x87_fpu_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_X87_FPU, isr, NULL);

    scheduler_start();
}

__NORETURN void alignment_check_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_ALIGNMENT_CHECK, isr, NULL);

    scheduler_start();
}

__NORETURN void machine_check_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_MACHINE_CHECK, isr, NULL);

    scheduler_start();
}

__NORETURN void simd_fpu_error_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_SIMD_FPU_ERROR, isr, NULL);

    scheduler_start();
}

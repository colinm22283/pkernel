#include <process/scheduler.h>

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

    interrupt_registry_invoke(IC_DIV0);

    scheduler_start();
}

__NORETURN void nmi_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_NMI);

    scheduler_start();
}

__NORETURN void bp_int3_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_BP_INT3);

    scheduler_start();
}

__NORETURN void ovf_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_OVF);

    scheduler_start();
}

__NORETURN void bound_range_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_BOUND_RANGE);

    scheduler_start();
}

__NORETURN void invalid_opcode_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_INVALID_OPCODE);

    scheduler_start();
}

__NORETURN void device_not_avail_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_DEVICE_NOT_AVAIL);

    scheduler_start();
}

__NORETURN void double_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_DOUBLE_FAULT);

    scheduler_start();
}

__NORETURN void coproc_segment_overrun_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_COPROC_SEGMENT_OVERRUN);

    scheduler_start();
}

__NORETURN void invalid_tss_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_INVALID_TSS);

    scheduler_start();
}

__NORETURN void segment_not_present_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_SEGMENT_NOT_PRESENT);

    scheduler_start();
}

__NORETURN void stack_segment_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_STACK_SEGMENT_FAULT);

    scheduler_start();
}

__NORETURN void general_protection_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_GENERAL_PROTECTION_FAULT);

    scheduler_start();
}

__NORETURN void page_fault_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_PAGE_FAULT);

    scheduler_start();
}

__NORETURN void x87_fpu_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_X87_FPU);

    scheduler_start();
}

__NORETURN void alignment_check_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_ALIGNMENT_CHECK);

    scheduler_start();
}

__NORETURN void machine_check_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_MACHINE_CHECK);

    scheduler_start();
}

__NORETURN void simd_fpu_error_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    interrupt_registry_invoke(IC_SIMD_FPU_ERROR);

    scheduler_start();
}

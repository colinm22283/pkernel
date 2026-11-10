#include <scheduler/scheduler.h>

#include <pman/pman.h>

#include <paging/kernel_translation.h>

#include <interrupt/interrupt_registry.h>

#include <sys/tsr/tsr.h>
#include <sys/tsr/tsr_print.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <sys/interrupt/interrupt_code.h>

#include <sys/exceptions/page_fault_error_code.h>

#include <defs.h>

#include <sys/debug/print.h>

__NORETURN void div0_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_DIV0, tsr, NULL);

    scheduler_yield();
}

__NORETURN void nmi_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_NMI, tsr, NULL);

    scheduler_yield();
}

__NORETURN void bp_int3_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_BP_INT3, tsr, NULL);

    scheduler_yield();
}

__NORETURN void ovf_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_OVF, tsr, NULL);

    scheduler_yield();
}

__NORETURN void bound_range_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_BOUND_RANGE, tsr, NULL);

    scheduler_yield();
}

__NORETURN void invalid_opcode_handler(task_state_record_t * tsr) {
    hlt();
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_INVALID_OPCODE, tsr, NULL);

    scheduler_yield();
}

__NORETURN void device_not_avail_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_DEVICE_NOT_AVAIL, tsr, NULL);

    scheduler_yield();
}

__NORETURN void double_fault_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_DOUBLE_FAULT, tsr, NULL);

    scheduler_yield();
}

__NORETURN void coproc_segment_overrun_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_COPROC_SEGMENT_OVERRUN, tsr, NULL);

    scheduler_yield();
}

__NORETURN void invalid_tss_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_INVALID_TSS, tsr, NULL);

    scheduler_yield();
}

__NORETURN void segment_not_present_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_SEGMENT_NOT_PRESENT, tsr, NULL);

    scheduler_yield();
}

__NORETURN void stack_segment_fault_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_STACK_SEGMENT_FAULT, tsr, NULL);

    scheduler_yield();
}

__NORETURN void general_protection_fault_handler(task_state_record_t * tsr) {
    hlt();
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_GENERAL_PROTECTION_FAULT, tsr, NULL);

    scheduler_yield();
}

__NORETURN void page_fault_handler(task_state_record_t * tsr, page_fault_error_code_t * error_code) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    tsr_print(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_PAGE_FAULT, tsr, error_code);

    scheduler_yield();
}

__NORETURN void x87_fpu_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_X87_FPU, tsr, NULL);

    scheduler_yield();
}

__NORETURN void alignment_check_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_ALIGNMENT_CHECK, tsr, NULL);

    scheduler_yield();
}

__NORETURN void machine_check_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_MACHINE_CHECK, tsr, NULL);

    scheduler_yield();
}

__NORETURN void simd_fpu_error_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = pman_kernel_context()->tlt_paddr;

    load_page_table(new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    interrupt_registry_invoke((interrupt_code_t) IC_SIMD_FPU_ERROR, tsr, NULL);

    scheduler_yield();
}


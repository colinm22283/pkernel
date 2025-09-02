#include <interrupt/interrupt_state_record.h>

#include <paging/kernel_translation.h>
#include <paging/tables.h>
#include <paging/manager.h>

#include <process/process.h>
#include <process/scheduler.h>

#include <sys/exceptions/page_fault_error_code.h>

#include <sys/paging/load_page_table.h>
#include <sys/paging/read_page_table.h>

#include <debug/vga_print.h>

void page_fault_handler(interrupt_state_record_t * isr, page_fault_error_code_t error_code) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    if (old_pml4t_paddr != new_pml4t_paddr) {
        load_page_table((void *) new_pml4t_paddr);

        process_load_isr(scheduler_current_process(), isr);
    }

    pman_page_fault_handler(isr, error_code);

    scheduler_start();
}

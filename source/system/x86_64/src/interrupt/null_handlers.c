#include <process/scheduler.h>

#include <paging/kernel_translation.h>

#include <sys/isr/isr.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <defs.h>

__NORETURN void null_handler(interrupt_state_record_t * isr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), isr);

    scheduler_start();
}

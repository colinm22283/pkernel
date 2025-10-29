#include <scheduler/scheduler.h>

#include <paging/kernel_translation.h>

#include <interrupt/interrupt_registry.h>

#include <sys/tsr/tsr.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <sys/interrupt/interrupt_code.h>

#include <defs.h>

__NORETURN void pic1_keyboard_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    interrupt_registry_invoke(IC_KEYBOARD, tsr, NULL);

    scheduler_yield();
}

__NORETURN void pic1_timer_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    if (old_pml4t_paddr != new_pml4t_paddr) scheduler_load_tsr(tsr);

    interrupt_registry_invoke(IC_TIMER, tsr, NULL);

    scheduler_yield();
}

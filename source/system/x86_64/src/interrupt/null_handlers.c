#include <scheduler/scheduler.h>

#include <paging/kernel_translation.h>

#include <sys/tsr/tsr.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <defs.h>

__NORETURN void null_handler(task_state_record_t * isr) {
    uint64_t new_pml4t_paddr = pman_kernel_context()->top_level_table_paddr;

    load_page_table((void *) new_pml4t_paddr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_isr(scheduler_current_process(), tsr);

    scheduler_yield();
}

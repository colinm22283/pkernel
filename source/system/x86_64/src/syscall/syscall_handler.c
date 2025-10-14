#include <paging/kernel_translation.h>

#include <scheduler/scheduler.h>

#include <syscall/syscall_handler.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

__NORETURN void sys_syscall_handler(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    // if (old_pml4t_paddr != new_pml4t_paddr) process_load_tsr(scheduler_current_process(), tsr);

    syscall_handler(tsr->rax, tsr->rsi, tsr->rdi, tsr->rdx, tsr->rcx, tsr->r8, tsr->r9, tsr);

    scheduler_yield();
}

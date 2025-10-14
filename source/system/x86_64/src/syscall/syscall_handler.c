#include <paging/kernel_translation.h>

#include <scheduler/scheduler.h>

#include <syscall/syscall_handler.h>

#include <sys/paging/read_page_table.h>
#include <sys/paging/load_page_table.h>

#include <sys/halt.h>

#include "debug/vga_print.h"

void syscall_handler_task(task_state_record_t * tsr) {
    vga_print("test\n");

    uint64_t return_val = syscall_handler(tsr->rax, tsr->rsi, tsr->rdi, tsr->rdx, tsr->rcx, tsr->r8, tsr->r9, tsr);

    scheduler_return_twin(return_val);
}

__NORETURN void syscall_handler_wrapper(task_state_record_t * tsr) {
    uint64_t old_pml4t_paddr = read_page_table();
    uint64_t new_pml4t_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    load_page_table((void *) new_pml4t_paddr);

    scheduler_load_tsr(tsr);

    scheduler_start_twin(syscall_handler_task);

    scheduler_yield();
}

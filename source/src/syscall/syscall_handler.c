#include <stdint.h>

#include <syscall/handlers/open.h>
#include <syscall/handlers/close.h>
#include <syscall/handlers/write.h>
#include <syscall/handlers/read.h>
#include <syscall/handlers/seek.h>
#include <syscall/handlers/exit.h>
#include <syscall/handlers/readdir.h>
#include <syscall/handlers/chdir.h>
#include <syscall/handlers/fork.h>
#include <syscall/handlers/exec.h>
#include <syscall/handlers/map.h>
#include <syscall/handlers/pipe.h>
#include <syscall/handlers/dup.h>
#include <syscall/handlers/mount.h>
#include <syscall/handlers/unmount.h>
#include <syscall/handlers/mkdir.h>
#include <syscall/handlers/remove.h>

#include <interface/interface_map.h>

#include <paging/kernel_translation.h>
#include <paging/tables.h>

#include <device/device.h>

#include <_process/address_translation.h>
#include <_process/scheduler.h>

#include <application/application_start_table.h>

#include <util/string/strlen.h>
#include <util/memory/memcpy.h>
#include <util/math/max.h>

#include <debug/vga_print.h>

#include <syscall_number.h>

#include <sys/paging/load_page_table.h>
#include <sys/paging/read_page_table.h>

uint64_t syscall_handler(uint64_t rax, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, interrupt_state_record_t * isr) {
    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    process_load_isr(scheduler_current_process(), isr);

    uint64_t return_value = 0;

    switch (rax) {
        case SYSCALL_OPEN: return_value = syscall_open((const char *) rsi, rdx); break;

        case SYSCALL_CLOSE: return_value = syscall_close((fd_t) rsi); break;

        case SYSCALL_WRITE: return_value = syscall_write((fd_t) rsi, (const char *) rdx, rcx); break;

        case SYSCALL_READ: return_value = syscall_read((fd_t) rsi, (char *) rdx, rcx); break;

        case SYSCALL_SEEK: return_value = syscall_seek((fd_t) rsi, (int64_t) rdx, rcx); break;

        case SYSCALL_EXIT: syscall_exit(rsi);

        case SYSCALL_READDIR: return_value = syscall_readdir((fd_t) rsi, (directory_entry_t *) rdx, rcx); break;

        case SYSCALL_CHDIR: return_value = syscall_chdir((const char *) rsi); break;

        case SYSCALL_FORK: return_value = syscall_fork(); break;

        case SYSCALL_EXEC: return_value = syscall_exec(
            (const char *) rsi,
            (const char **) rdx,
            (uint64_t) rcx
        ); break;

        case SYSCALL_MAP: return_value = (uint64_t) syscall_map((fd_t) rsi, (void *) rdx, rcx, r8, r9); break;

        case SYSCALL_WAIT: {
            process_t * current_process = scheduler_current_process();
            process_thread_t * current_thread = scheduler_current_thread();

            current_thread->state = TS_WAIT_CHILD;
        } break;

        case SYSCALL_PIPE: return_value = syscall_pipe((fd_t *) rsi, (open_options_t) rdx); break;

        case SYSCALL_DUP: return_value = syscall_dup((fd_t) rsi, (fd_t) rdx); break;

        case SYSCALL_MOUNT: return_value = syscall_mount(
            (const char *) rsi,
            (const char *) rdx,
            (const char *) rcx,
            (mount_options_t) r8,
            (const char *) r9
        ); break;

        case SYSCALL_UNMOUNT: return_value = syscall_unmount((const char *) rsi); break;

        case SYSCALL_MKDIR: return_value = syscall_mkdir((const char *) rsi); break;

        case SYSCALL_REMOVE: return_value = syscall_remove((const char *) rsi); break;

        default: return_value = ERROR_BAD_SYSCALL; break;
    }

    scheduler_current_thread()->isr.rax = return_value;

    scheduler_start();
}

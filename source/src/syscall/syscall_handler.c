#include <scheduler/scheduler.h>

#include <timer/timer.h>

#include <syscall/syscall_handler.h>
#include <syscall/syscall_names.h>
#include <syscall/syscall_debug.h>

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
#include <syscall/handlers/socket.h>
#include <syscall/handlers/connect.h>
#include <syscall/handlers/bind.h>
#include <syscall/handlers/listen.h>
#include <syscall/handlers/accept.h>
#include <syscall/handlers/signal.h>

#include <sys/debug/print.h>

uint64_t syscall_handler(
    uint64_t syscall_number,
    uint64_t arg0,
    uint64_t arg1,
    uint64_t arg2,
    uint64_t arg3,
    uint64_t arg4,
    task_state_record_t * tsr
) {
    syscall_debug_print_hex(scheduler_current_process()->id);
    syscall_debug_print(" -> ");
    if (syscall_number < _SYSCALL_COUNT) syscall_debug_print(syscall_names[syscall_number]);
    else syscall_debug_print("SYSCALL_UNKNOWN");
    syscall_debug_print(": ");
    syscall_debug_print_hex(arg0);
    syscall_debug_print(" ");
    syscall_debug_print_hex(arg1);
    syscall_debug_print(" ");
    syscall_debug_print_hex(arg2);
    syscall_debug_print(" ");
    syscall_debug_print_hex(arg3);
    syscall_debug_print(" ");
    syscall_debug_print_hex(arg4);
    syscall_debug_print(" -> ");

    syscall_debug_print_hex(get_root_mapping(scheduler_current_thread()->stack_mapping)->alloc.palloc.paddrs[0]);
    syscall_debug_print("\n");


    switch (syscall_number) {
        case SYSCALL_OPEN: return syscall_open((const char *) arg0, arg1);

        case SYSCALL_CLOSE: return syscall_close((fd_t) arg0);

        case SYSCALL_WRITE: return syscall_write((fd_t) arg0, (const char *) arg1, arg2);

        case SYSCALL_READ: return syscall_read((fd_t) arg0, (char *) arg1, arg2);

        // case SYSCALL_SEEK: return syscall_seek((fd_t) arg0, (int64_t) arg1, arg2);

        case SYSCALL_EXIT: syscall_exit(arg0); return 0;

        case SYSCALL_READDIR: return syscall_readdir((fd_t) arg0, (directory_entry_t *) arg1, arg2);

        case SYSCALL_CHDIR: return syscall_chdir((const char *) arg0);

        case SYSCALL_FORK: return syscall_fork();

        case SYSCALL_EXEC: return syscall_exec((const char *) arg0, (const char **) arg1, (uint64_t) arg2);

        case SYSCALL_MAP: return (uint64_t) syscall_map((fd_t) arg0, (void *) arg1, arg2, arg3, arg4);

        case SYSCALL_WAIT: {
            process_t * current_process = scheduler_current_process();

            scheduler_await(current_process->child_finished);

            return ERROR_OK;
        }

        case SYSCALL_PIPE: return syscall_pipe((fd_t *) arg0, (open_options_t) arg1);

        case SYSCALL_DUP: return syscall_dup((fd_t) arg0, (fd_t) arg1);

        case SYSCALL_MOUNT: return syscall_mount(
            (const char *) arg0,
            (const char *) arg1,
            (const char *) arg2,
            (mount_options_t) arg3,
            (const char *) arg4
        );

        case SYSCALL_UNMOUNT: return syscall_unmount((const char *) arg0);

        case SYSCALL_MKDIR: return syscall_mkdir((const char *) arg0);

        case SYSCALL_REMOVE: return syscall_remove((const char *) arg0);

        case SYSCALL_OPENAT: return syscall_openat((fd_t) arg0, (const char *) arg1, (open_options_t) arg2);

        case SYSCALL_SOCKET: return syscall_socket((socket_domain_t) arg0, (socket_type_t) arg1, (uint64_t) arg2);

        case SYSCALL_CONNECT: return syscall_connect((fd_t) arg0, (const sockaddr_t *) arg1, (size_t) arg2);

        case SYSCALL_BIND: return syscall_bind((fd_t) arg0, (const sockaddr_t *) arg1, (size_t) arg2);

        case SYSCALL_LISTEN: return syscall_listen((fd_t) arg0, (size_t) arg1);

        case SYSCALL_ACCEPT: return syscall_accept((fd_t) arg0);

        case SYSCALL_SIGNAL: return syscall_signal((signal_number_t) arg0, (signal_handler_t *) arg1);

        default: return ERROR_BAD_SYSCALL;
    }
}


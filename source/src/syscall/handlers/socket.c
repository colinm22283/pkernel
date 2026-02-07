#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/socket.h>

#include <debug/vga_print.h>

fd_t syscall_socket(socket_domain_t domain, socket_type_t type, uint64_t protocol) {
    process_t * current_process = scheduler_current_process();

    fs_directory_entry_t * dirent = fs_make_anon_socket(domain, type, protocol);
    if (dirent == NULL) return ERROR_UNKNOWN;

    fd_t fd = file_table_open(&current_process->file_table, dirent, OPEN_WRITE | OPEN_READ);

    return fd;
}


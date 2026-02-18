#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/kill.h>

error_number_t syscall_kill(pid_t pid) {
    process_t * target_proc = process_lookup(pid);
    if (target_proc == NULL) return ERROR_PROC_NOT_FOUND;

    process_kill(target_proc);

    return ERROR_OK;
}



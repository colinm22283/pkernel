#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/kill.h>

int syscall_kill(pid_t pid) {
    process_t * target_proc = process_lookup(pid);
    if (target_proc == NULL) return -ESRCH;

    process_kill(target_proc);

    return 0;
}



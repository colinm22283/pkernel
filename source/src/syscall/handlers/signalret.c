#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/signalret.h>

#include <util/memory/memcpy.h>

#include <sys/function/pop_tsr.h>
#include <sys/debug/print.h>

#include <debug/printf.h>

int syscall_signalret(void) {
    thread_t * kern_thread = scheduler_current_thread();
    thread_t * thread = kern_thread->twin_thread;
    process_t * process = thread->process;

    pop_tsr(process, &thread->tsr);

    kern_thread->state = TS_STOPPED;
    thread_run(thread);

    scheduler_yield();
}


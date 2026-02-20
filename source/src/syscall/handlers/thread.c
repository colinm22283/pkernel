#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/thread.h>

#include <util/memory/memcpy.h>

#include <sys/tsr/tsr_load_return.h>

error_number_t syscall_thread(void * handler) {
    thread_t * current_thread = scheduler_current_thread();
    process_t * current_process = current_thread->process;

    thread_t * new_thread = thread_create_user(current_process->paging_context, current_process);

    process_add_thread(current_process, new_thread);

    thread_load_pc(new_thread, handler);

    thread_run(new_thread);

    return ERROR_OK;
}


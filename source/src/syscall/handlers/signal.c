#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <syscall/handlers/signal.h>

error_number_t syscall_signal(signal_number_t sig, signal_handler_t * handler) {
    process_t * current_process = scheduler_current_process();

    return signal_table_set(&current_process->signal_table, sig, handler);
}


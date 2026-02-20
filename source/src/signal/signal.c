#include <stddef.h>

#include <scheduler/scheduler.h>

#include <signal/signal.h>

#include <sys/function/push_function.h>
#include <sys/function/push_tsr.h>

#include <sys/debug/print.h>
#include <debug/printf.h>

void signal_table_init(signal_table_t * st) {
    for (size_t i = 0; i < _SIG_COUNT; i++) {
        st->handlers[i].action = signal_defaults[i];
        st->handlers[i].user_handler = NULL;
    }

    st->signal_queue_size = 0;
    st->signal_queue_head = 0;
    st->signal_queue_tail = 0;
}

void signal_table_free(signal_table_t * st) { }

error_number_t signal_table_invoke(process_t * process, signal_number_t sig) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    signal_table_t * st = &process->signal_table;

    if (st->signal_queue_size == SIGNAL_QUEUE_SIZE) return ERROR_SIG_FULL;

    st->signal_queue[st->signal_queue_tail] = sig;

    st->signal_queue_tail = (st->signal_queue_tail + 1) % SIGNAL_QUEUE_SIZE;
    st->signal_queue_size++;

    thread_t * thread = NULL;
    for (size_t i = 0; i < process->thread_count; i++) {
        thread = process->threads[i];

        if (thread->state != TS_INTERRUPTABLE_WAIT) {
            thread = NULL;

             break;
        }
    }

    if (thread != NULL) {
        thread_interrupt(thread);
    }

    return ERROR_OK;
}

error_number_t signal_table_set(signal_table_t * st, signal_number_t sig, signal_handler_t * handler) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    signal_t * signal = &st->handlers[sig];

    signal->user_handler = handler;

    return ERROR_OK;
}

error_number_t signal_table_reset(signal_table_t * st, signal_number_t sig) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    signal_t * signal = &st->handlers[sig];

    signal->user_handler = NULL;

    return ERROR_OK;
}

error_number_t signal_table_resume(thread_t * thread) {
    process_t * process = thread->process;

    signal_table_t * st = &process->signal_table;

    if (st->signal_queue_size == 0) return ERROR_SIG_EMPTY;

    signal_number_t sig = st->signal_queue[st->signal_queue_head];

    st->signal_queue_head = (st->signal_queue_head + 1) % SIGNAL_QUEUE_SIZE;
    st->signal_queue_size--;

    signal_t * signal = &st->handlers[sig];

    if (signal->user_handler == NULL) {
        switch (signal->action) {
            case ACT_TERMINATE: {
                process_kill(process);
            } break;

            case ACT_CONTINUE: {
                // TODO: resume process if stopped
            } break;

            case ACT_IGNORE: break;

            case ACT_STOP: {
                // TODO: stop process
            } break;
        }
    }
    else {
        arg_t args[1] = { sig };

        push_tsr(thread->process, &thread->tsr);
        push_function(thread->process, &thread->tsr, signal->user_handler, args, 1);
    }

    return ERROR_OK;
}

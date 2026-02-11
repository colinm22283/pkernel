#include <stddef.h>

#include <scheduler/scheduler.h>

#include <signal/signal.h>

#include <sys/function/push_function.h>

#include <sys/debug/print.h>

void signal_table_init(signal_table_t * st) {
    for (size_t i = 0; i < _SIG_COUNT; i++) {
        st->handlers[i].action = signal_defaults[i];
        st->handlers[i].user_handler = NULL;
    }
}

void signal_table_free(signal_table_t * st) {

}

error_number_t signal_table_invoke(process_t * process, signal_number_t sig, thread_t * handling_thread) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    if (handling_thread == NULL) handling_thread = process->threads[0];

    signal_table_t * st = &process->signal_table;

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

        thread_push_function(handling_thread, signal->user_handler, args, 1);
    }

    return ERROR_OK;
}

error_number_t signal_table_set(signal_table_t * st, signal_number_t sig, signal_handler_t * handler) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    signal_t * signal = &st->handlers[sig];

    signal->user_handler = handler;

    return ERROR_OK;
}

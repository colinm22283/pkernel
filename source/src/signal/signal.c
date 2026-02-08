#include <stddef.h>

#include <scheduler/scheduler.h>

#include <signal/signal.h>

#include <sys/function/push_function.h>

#include <sys/debug/print.h>

void signal_table_init(signal_table_t * st) {
    st->handlers[SIG_ABORT].default_action = ACT_TERMINATE;
    st->handlers[SIG_ABORT].user_handler = NULL;

    st->handlers[SIG_ALARM].default_action = ACT_TERMINATE;
    st->handlers[SIG_ALARM].user_handler = NULL;

    st->handlers[SIG_PAGE].default_action = ACT_TERMINATE;
    st->handlers[SIG_PAGE].user_handler = NULL;
}

void signal_table_free(signal_table_t * st) {

}

error_number_t signal_table_invoke(signal_table_t * st, signal_number_t sig, thread_t * handling_thread) {
    if (sig >= _SIG_COUNT) return ERROR_NO_SIG;

    signal_t * signal = &st->handlers[sig];

    debug_print("SIG\n");

    if (signal->user_handler == NULL) {
    }
    else {
        arg_t args[1] = { sig };

        debug_print("INVOKING AT 0x");
        debug_print_hex((intptr_t) signal->user_handler);
        debug_print("\n");

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

#include <stddef.h>

#include <process/process.h>
#include <process/signal.h>

#include <process/address_translation.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

#include <debug/vga_print.h>

void signal_table_init(signal_table_t * st) {
    st->head.next = &st->tail;
    st->head.prev = NULL;
    st->tail.next = NULL;
    st->tail.prev = &st->head;

    st->handler_count = 0;
    st->handler_capacity = 1;
    st->handlers = heap_alloc(st->handler_capacity * sizeof(signal_table_handler_t));
}

void signal_table_free(signal_table_t * st) {
    signal_table_signal_t * signal = st->head.next;

    while (signal != &st->tail) {
        signal_table_signal_t * next = signal->next;

        heap_free(signal);

        signal = next;
    }

    heap_free(st->handlers);
}

void signal_table_register(signal_table_t * st, uint64_t rip) {
    signal_table_handler_t * handler = &st->handlers[st->handler_count++];

    handler->signal_rip = rip;

    if (st->handler_count == st->handler_capacity) {
        st->handler_capacity *= 2;

        st->handlers = heap_realloc(st->handlers, st->handler_capacity * sizeof(signal_table_handler_t));
    }
}

void signal_table_invoke(signal_table_t * st, signal_number_t number, void * cookie) {
    signal_table_signal_t * signal = heap_alloc(sizeof(signal_table_signal_t));

    signal->number = number;
    signal->cookie = cookie;

    signal->next = st->head.next;
    signal->prev = &st->head;
    st->head.next->prev = signal;
    st->head.next = signal;
}

void signal_table_enter(signal_table_t * st, process_t * process) {
    signal_table_signal_t * signal = st->tail.prev;

    if (signal != &st->head) {
        vga_print("SIGNAL\n");

        signal->prev->next = signal->next;
        signal->next->prev = signal->prev;

        process_thread_t * thread = process->thread_table.threads[process->thread_table.current_index];

        void * new_rsp = (void *) (thread->isr.rsp + sizeof(interrupt_state_record_t) + 8);

        //pman_mapping_t * _rsp_mapping = pman_context_get_vaddr(process->paging_context, new_rsp);

        //pman_mapping_t * rsp_mapping = pman_context_prepare_write(process, _rsp_mapping);

        void * rsp_kernel_mapping = process_user_to_kernel(process, new_rsp);

        memcpy(rsp_kernel_mapping + sizeof(uint64_t), new_rsp, sizeof(interrupt_state_record_t));
        memcpy(rsp_kernel_mapping, new_rsp, sizeof(uint64_t));

        thread->isr.rsp += sizeof(interrupt_state_record_t) + sizeof(uint64_t);


    }
}

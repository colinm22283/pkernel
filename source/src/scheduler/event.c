#include <stddef.h>

#include <scheduler/scheduler.h>
#include <scheduler/event.h>

#include <util/heap/heap.h>

#include <config/event.h>

#ifdef EVENT_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("event");

event_t * event_init(void) {
    event_t * event = heap_alloc_debug(sizeof(event_t), "event");

    kprintf("event_init()");

    event->has_signal = false;

    event->waiter_head.next = &event->waiter_tail;
    event->waiter_head.prev = NULL;
    event->waiter_tail.next = NULL;
    event->waiter_tail.prev = &event->waiter_head;

    return event;
}

void event_free(event_t * event) {
    kprintf("event_free()");
    heap_free(event);
}

void event_invoke(event_t * event) {
    kprintf("event_invoke()");

    if (event->waiter_head.next != &event->waiter_tail) {
        waiter_t * waiter = event->waiter_head.next;
        thread_t * thread = waiter->thread;

        waiter_free(waiter);

        thread->waiter = NULL;
        thread->event = NULL;

        thread_run(thread);
    }
    else {
        event->has_signal = true;
    }
}

void event_interrupt(event_t * event) {
    kprintf("event_interrupt()");

    while (event->waiter_head.next != &event->waiter_tail) { // TODO: loop?
        waiter_t * waiter = event->waiter_head.next;
        thread_t * thread = waiter->thread;

        waiter_free(waiter);

        thread->waiter = NULL;
        thread->event = NULL;

        heap_free(waiter);

        thread_run(thread);
        thread->state = TS_INTERRUPTED;
    }
}

void waiter_free(waiter_t * waiter) {
    waiter->next->prev = waiter->prev;
    waiter->prev->next = waiter->next;

    heap_free(waiter);
}

#include <stddef.h>

#include <scheduler/scheduler.h>
#include <scheduler/event.h>

#include <util/heap/heap.h>

#include "debug/vga_print.h"

event_t * event_init(void) {
    event_t * event = heap_alloc(sizeof(event_t));

    event->waiter_head.next = &event->waiter_tail;
    event->waiter_head.prev = NULL;
    event->waiter_tail.next = NULL;
    event->waiter_tail.prev = &event->waiter_head;

    return event;
}

void event_free(event_t * event) {
    heap_free(event);
}

void event_invoke(event_t * event) {
    if (event->waiter_head.next != &event->waiter_tail) {
        vga_print("Found thread\n");

        waiter_t * waiter = event->waiter_head.next;

        event->waiter_head.next = event->waiter_head.next->next;
        event->waiter_head.next->prev = &event->waiter_head;

        thread_run(waiter->thread);
    }
}


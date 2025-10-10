#include <stddef.h>

#include <event/event.h>

#include <process/scheduler.h>

#include <util/heap/heap.h>
#include <util/memory/memset.h>

#include <sys/isr/store_isr.h>

event_waiter_t waiter_queue_head, waiter_queue_tail;

event_t * event_init(void) {
    event_t * event = heap_alloc(sizeof(event_t));

    event->head.next = &event->tail;
    event->head.prev = NULL;
    event->tail.next = NULL;
    event->tail.prev = &event->head;

    return event;
}

error_number_t event_free(event_t * event) {
    event_waiter_t * waiter = event->head.next;
    while (waiter != &event->tail) {
        event_waiter_t * next = waiter->next;

        heap_free(waiter);

        waiter = next;
    }

    heap_free(event);

    return ERROR_OK;
}

void event_await(process_thread_t * thread, event_t * event) {
    event_waiter_t * waiter = heap_alloc(sizeof(event_waiter_t));

    waiter->event = event;

    waiter->prev = &event->head;
    waiter->next = event->head.next;
    event->head.next->prev = waiter;
    event->head.next = waiter;

    thread->state = TS_WAIT_EVENT;
    thread->wait_info.event.waiter = waiter;

    store_isr(&waiter->isr);

    scheduler_start();
}

void event_invoke_once(event_t * event) {
    if (event->head.next == &event->tail) return;

    event_waiter_t * waiter = event->head.next;

    event->head.next = event->head.next->next;
    event->head.next->prev = &event->head;

    waiter->next = waiter_queue_head.next;
    waiter->prev = &waiter_queue_head;

    waiter_queue_head.next->prev = waiter;
    waiter_queue_head.next = waiter;
}

void event_manager_init(void) {
    waiter_queue_head.next = &waiter_queue_tail;
    waiter_queue_head.prev = NULL;
    waiter_queue_tail.next = NULL;
    waiter_queue_tail.prev = &waiter_queue_head;
}

void event_manager_resume(void) {
    if (waiter_queue_head.next == &waiter_queue_tail) return;

    event_waiter_t * waiter = waiter_queue_head.next;

    waiter_queue_head.next = waiter_queue_head.next->next;
    waiter_queue_head.next->prev = &waiter_queue_head;

    event_waiter_resume_and_free(waiter);
}
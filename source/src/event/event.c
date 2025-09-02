#include <event/event.h>

#include <util/heap/heap.h>

event_t * event_init(void) {
    event_t * event = heap_alloc(sizeof(event_t));

    return event;
}

error_number_t event_free(event_t * event) {
    heap_free(event);

    return ERROR_OK;
}

event_waiter_t * event_await(event_t * event) {

}

void event_invoke_once(event_t * event) {

}

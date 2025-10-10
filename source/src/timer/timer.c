#include <stddef.h>

#include <timer/timer.h>

#include <interrupt/interrupt_registry.h>

#include <util/heap/heap.h>

#include <util/memory/memset.h>

#include <sys/asm/cli.h>
#include <sys/asm/sti.h>
#include <sys/halt.h>

timer_t timers_head;
timer_t timers_tail;

timer_t * timer_init(timer_handler_t handler, void * cookie, uint64_t start_ticks, uint64_t interval_ticks) {
    timer_t * timer = heap_alloc(sizeof(timer_t));

    timer->state = WAITING;

    timer->current_ticks = 0;
    timer->start_ticks = start_ticks;
    timer->interval_ticks = interval_ticks;

    timer->handler = handler;
    timer->cookie = cookie;

    timer->next = timers_head.next;
    timer->prev = &timers_head;

    timers_head.next->prev = timer;
    timers_head.next = timer;

    return timer;
}

void timer_free(timer_t * timer) {
    timer->next->prev = timer->prev;
    timer->prev->next = timer->next;

    heap_free(timer);
}

static inline void timer_update(timer_t * timer) {
    timer->current_ticks++;

    switch (timer->state) {
        case WAITING: {
            if (timer->current_ticks >= timer->start_ticks) {
                timer->state = RUNNING;
                timer->handler(timer);

                timer->current_ticks -= timer->start_ticks;
                return;
            }
        } break;

        case RUNNING: {
            if (timer->current_ticks >= timer->interval_ticks) {
                timer->handler(timer);

                timer->current_ticks -= timer->interval_ticks;
                return;
            }
        } break;
    }
}

uint64_t timers_interrupt_count = 0;
void timers_interrupt_handler(__MAYBE_UNUSED interrupt_code_t channel, __MAYBE_UNUSED task_state_record_t * isr, __MAYBE_UNUSED void * error_code) {
    timers_update();

    timers_interrupt_count = 0;
}

void timers_init(void) {
    timers_interrupt_count = 0;

    timers_head.prev = NULL;
    timers_head.next = &timers_tail;

    timers_tail.prev = &timers_head;
    timers_tail.next = NULL;

    interrupt_registry_register(IC_TIMER, timers_interrupt_handler);
}

void timers_update(void) {
    timer_t * timer = timers_head.next;

    while (timer != &timers_tail) {
        timer_update(timer);

        timer = timer->next;
    }
}

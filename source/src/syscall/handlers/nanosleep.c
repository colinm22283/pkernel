#include <stddef.h>

#include <scheduler/event.h>
#include <scheduler/scheduler.h>

#include <process/process.h>

#include <timer/timer.h>

#include <debug/printf.h>

#include <syscall/handlers/nanosleep.h>

void nanosleep_timer_handler(timer_t * timer) {
    event_t * event = timer->cookie;

    event_invoke(event);

    timer_free(timer);
}

int syscall_nanosleep(size_t nanos) {
    event_t * event = event_init();

    timer_init(nanosleep_timer_handler, event, TIMER_NS_TO_TICKS(nanos), 0);

    scheduler_await(event);
    event_free(event);

    return 0;
}


#include <stddef.h>

#include <scheduler/scheduler.h>

#include <process/process.h>

#include <timer/timer.h>

#include <syscall/handlers/alarm.h>

void alarm_timer_handler(timer_t * timer) {
    process_t * process = timer->cookie;

    signal_table_invoke(process, SIG_ALARM);

    timer_free(timer);
}

int syscall_alarm(size_t seconds) {
    process_t * current_process = scheduler_current_process();

    timer_init(alarm_timer_handler, current_process, TIMER_S_TO_TICKS(seconds), 0);

    return ERROR_OK;
}


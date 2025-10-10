#include <scheduler/scheduler.h>
#include <scheduler/core.h>

#include <timer/timer.h>

#include <sys/interrupt/wait_for_interrupt.h>

#define SCHEDULER_QUANTUM (0)

typedef struct {
    thread_t head, tail;
} scheduler_queue_t;

scheduler_queue_t scheduler_queues[SP_COUNT];

void scheduler_init(void) {
    scheduler_cores_init();

    for (size_t i = 0; i < SP_COUNT; i++) {
        scheduler_queues[i].head.next = &scheduler_queues[i].tail;
        scheduler_queues[i].head.prev = NULL;
        scheduler_queues[i].tail.next = NULL;
        scheduler_queues[i].tail.prev = &scheduler_queues[i].head;
    }
}

__NORETURN void scheduler_yield(void) {
    while (1) {
        for (size_t i = 0; i < SP_COUNT; i++) {
            if (scheduler_queues[i].head.next != &scheduler_queues[i].tail) {

            }
        }

        wait_for_interrupt();
    }
}

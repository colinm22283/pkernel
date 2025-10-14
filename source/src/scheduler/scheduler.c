#include <scheduler/scheduler.h>
#include <scheduler/core.h>

#include <timer/timer.h>

#include <sys/interrupt/wait_for_interrupt.h>

#define SCHEDULER_QUANTUM (0)

typedef struct {
    thread_t head, tail;
} scheduler_queue_t;

scheduler_queue_t scheduler_queues[TP_COUNT];

void scheduler_init(void) {
    scheduler_cores_init();

    for (size_t i = 0; i < TP_COUNT; i++) {
        scheduler_queues[i].head.next = &scheduler_queues[i].tail;
        scheduler_queues[i].head.prev = NULL;
        scheduler_queues[i].tail.next = NULL;
        scheduler_queues[i].tail.prev = &scheduler_queues[i].head;
    }
}

void scheduler_queue(thread_t * thread) {
    scheduler_queue_t * queue = &scheduler_queues[thread->priority];

    thread->prev = queue->tail.prev;
    thread->next = &queue->tail;

    queue->tail.prev->next = thread;
    queue->tail.prev = thread;
}

__NORETURN void scheduler_yield(void) {
    scheduler_core_t * current_core = scheduler_current_core();

    if (current_core->current_thread != NULL) {
        scheduler_queue(current_core->current_thread);

        current_core->current_thread = NULL;
    }

    while (1) {
        for (size_t i = 0; i < TP_COUNT; i++) {
            while (scheduler_queues[i].head.next != &scheduler_queues[i].tail) {
                thread_t * thread = scheduler_queues[i].head.next;

                thread->prev->next = thread->next;
                thread->next->prev = thread->prev;

                switch (thread->state) {
                    case TS_RUNNING: {
                        current_core->current_thread = thread;

                        thread_resume(thread);
                    } break;

                    case TS_STOPPED: break;

                    case TS_DEAD: {
                        thread_free(thread);
                    } break;
                }
            }
        }

        wait_for_interrupt();
    }
}

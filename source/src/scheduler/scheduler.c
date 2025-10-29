#include <scheduler/scheduler.h>
#include <scheduler/core.h>

#include <timer/timer.h>

#include <sys/interrupt/wait_for_interrupt.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

#include <sys/tsr/store_tsr_and_yield.h>
#include <sys/tsr/tsr_set_stack.h>
#include <sys/tsr/tsr_load_task.h>
#include <sys/tsr/tsr_load_return.h>

#include <sys/halt.h>

#include <debug/vga_print.h>

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
    if (thread->state == TS_RUNNING || thread->state == TS_DEAD) {
        scheduler_queue_t * queue = &scheduler_queues[thread->priority];

        thread->prev = queue->tail.prev;
        thread->next = &queue->tail;

        queue->tail.prev->next = thread;
        queue->tail.prev = thread;
    }
}

void scheduler_await(event_t * event) {
    if (!event->has_signal) {
        scheduler_core_t * current_core = scheduler_current_core();

        current_core->current_thread->state = TS_WAITING;

        waiter_t * waiter = heap_alloc(sizeof(waiter_t));

        waiter->thread = current_core->current_thread;

        waiter->next = &event->waiter_tail;
        waiter->prev = event->waiter_tail.prev;
        event->waiter_tail.prev->next = waiter;
        event->waiter_tail.prev = waiter;

        store_tsr_and_yield(&current_core->current_thread->tsr);
    }
    else {
        event->has_signal = false;
    }
}

void scheduler_load_tsr(task_state_record_t * tsr) {
    scheduler_core_t * current_core = scheduler_current_core();

    memcpy(&current_core->current_thread->tsr, tsr, sizeof(task_state_record_t));
}

void scheduler_start_twin(void (*task_handler)(task_state_record_t * tsr)) {
    scheduler_core_t * current_core = scheduler_current_core();

    current_core->current_thread->state = TS_WAITING;

    thread_t * twin = current_core->current_thread->twin_thread;
    thread_t * user_thread = current_core->current_thread;

    current_core->current_thread = NULL;

    tsr_load_task(&twin->tsr, &user_thread->tsr, task_handler);
    tsr_set_stack(
        &twin->tsr,
        twin->stack_mapping->vaddr,
        twin->stack_mapping->size_pages * 0x1000
    );

    thread_run(twin);
}

__NORETURN void scheduler_return_twin(uint64_t ret_val) {
    scheduler_core_t * current_core = scheduler_current_core();

    current_core->current_thread->state = TS_STOPPED;

    thread_t * twin = current_core->current_thread->twin_thread;

    current_core->current_thread = NULL;

    if (twin->state == TS_DEAD) {
        scheduler_queue(twin);
        scheduler_yield();
    }
    else {
        tsr_load_return(&twin->tsr, ret_val);

        thread_run(twin);
        scheduler_yield();
    }
}

__NORETURN void scheduler_yield(void) {
    // size_t proc_count = 0;
    // for (size_t i = 0; i < TP_COUNT; i++) {
    //     thread_t * thread = scheduler_queues[i].head.next;
    //     while (thread != &scheduler_queues[i].tail) {
    //         proc_count++;
    //
    //         thread = thread->next;
    //     }
    // }

    // vga_print("yield ");
    // vga_print_hex(proc_count);
    // vga_print("\n");

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

                    case TS_WAITING: case TS_STOPPED: break;

                    case TS_DEAD: {
                        thread_free(thread);
                    } break;
                }
            }
        }

        wait_for_interrupt();
    }
}

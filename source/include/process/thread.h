#pragma once

#include <stddef.h>

#include <paging/manager.h>

#include <sys/tsr/tsr.h>
#include <sys/function/arg.h>

#define DEFAULT_THREAD_STACK_SIZE (0x2000)

struct process_s;

typedef enum {
    TL_USER,
    TL_KERNEL,
} thread_level_t;

typedef enum {
    TS_RUNNING,
    TS_STOPPED,
    TS_WAITING,
    TS_UNINTERRUPTABLE_WAIT,
    TS_INTERRUPTABLE_WAIT,
    TS_INTERRUPTED,
    TS_DEAD,
} thread_state_t;

typedef enum {
    TP_CRITICAL,
    TP_HIGH,
    TP_MEDIUM,
    TP_LOW,

    TP_COUNT,
} thread_priority_t;

typedef struct thread_s {
    struct process_s * process;

    thread_level_t level;
    thread_state_t state;
    thread_priority_t priority;

    pman_mapping_t * stack_mapping;

    task_state_record_t tsr;

    struct thread_s * twin_thread;

    struct waiter_s * waiter;
    struct event_s * event;

    struct thread_s * next;
    struct thread_s * prev;
} thread_t;

thread_t * thread_create_user(pman_context_t * pman_context, struct process_s * parent);
thread_t * thread_create_fork(pman_context_t * user_context, struct process_s * parent, thread_t * target);
thread_t * thread_create_kernel(void);

void thread_free(thread_t * thread);

void thread_run(thread_t * thread);
void thread_kill(thread_t * thread);
void thread_load_pc(thread_t * thread, void * pc);

error_number_t thread_interrupt(thread_t * thread);

__NORETURN void thread_resume(thread_t * thread);
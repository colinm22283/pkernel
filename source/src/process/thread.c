#include <stddef.h>

#include <process/thread.h>
#include <process/process.h>

#include <scheduler/scheduler.h>

#include <util/heap/heap.h>

#include <util/memory/memset.h>
#include <util/memory/memcpy.h>

#include <sys/tsr/resume_tsr.h>
#include <sys/tsr/tsr_set_stack.h>
#include <sys/tsr/tsr_load_pc.h>
#include <sys/paging/page_size.h>

#include <sys/function/push_function.h>

#include <sys/panic.h>

#include <config/thread.h>

#ifdef THREAD_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("thread");

thread_t * thread_create_user(pman_context_t * user_context, process_t * parent) {
    thread_t * thread = heap_alloc_debug(sizeof(thread_t), "thread user");

    kprintf("thread_create_user()");

    thread->process = parent;

    thread->level = TL_USER;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = thread_create_kernel();
    thread->twin_thread->twin_thread = thread;
    thread->twin_thread->process = parent;

    pman_range_t * stack_range = pman_add_anon_map(user_context, NULL, DEFAULT_THREAD_STACK_SIZE, 0, PMAN_WRITE);
    thread->stack_vaddr = stack_range->vaddr;
    thread->stack_size  = DEFAULT_THREAD_STACK_SIZE;
    pman_range_free(stack_range);

    memset(&thread->tsr, 0, sizeof(task_state_record_t));

    tsr_set_stack(&thread->tsr, thread->stack_vaddr, thread->stack_size * PAGE_SIZE);

    thread->waiter = NULL;
    thread->event = NULL;

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

thread_t * thread_create_fork(pman_context_t * user_context, process_t * parent, thread_t * target) {
    thread_t * thread = heap_alloc_debug(sizeof(thread_t), "thread fork");

    kprintf("thread_create_fork()");

    thread->process = parent;

    thread->level = TL_USER;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = thread_create_kernel();
    thread->twin_thread->twin_thread = thread;
    thread->twin_thread->process = parent;

    thread->stack_vaddr = target->stack_vaddr;
    thread->stack_size  = target->stack_size;

    memcpy(&thread->tsr, &target->tsr, sizeof(task_state_record_t));

    thread->waiter = NULL;

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

thread_t * thread_create_kernel(void) {
    thread_t * thread = heap_alloc_debug(sizeof(thread_t), "thread kernel");

    kprintf("thread_create_kernel()");

    thread->process = NULL;

    thread->level = TL_KERNEL;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = NULL;

    pman_range_t * stack_range = pman_add_anon_map(pman_kernel_context(), NULL, DEFAULT_THREAD_STACK_SIZE, 0, PMAN_WRITE);
    thread->stack_vaddr = stack_range->vaddr;
    thread->stack_size  = DEFAULT_THREAD_STACK_SIZE;
    pman_range_free(stack_range);

    tsr_set_stack(&thread->tsr, thread->stack_vaddr, thread->stack_size);

    kprintf("THREAD STACK MAPPING: %p", thread->stack_vaddr);

    memset(&thread->tsr, 0, sizeof(task_state_record_t));

    thread->waiter = NULL;

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

void thread_free(thread_t * thread) {
    kprintf("thread_free()");

    if (thread->twin_thread != NULL) {
        thread->twin_thread->twin_thread = NULL;

        thread_free(thread->twin_thread);
    }

    if (thread->state == TS_UNINTERRUPTABLE_WAIT) {
        waiter_free(thread->waiter);
    }

    if (thread->level == TL_KERNEL) {
        pman_range_unmap(pman_get_range(pman_kernel_context(), thread->stack_vaddr, thread->stack_size));
    }
    else if (thread->level == TL_USER) {
        pman_range_unmap(pman_get_range(thread->process->paging_context, thread->stack_vaddr, thread->stack_size));
    }

    heap_free(thread);
}

void thread_run(thread_t * thread) {
    // if (thread->twin_thread != NULL) thread->twin_thread->state = TS_STOPPED;

    if (thread->state != TS_RUNNING) {
        thread->state = TS_RUNNING;

        scheduler_queue(thread);
    }
}

void thread_kill(thread_t * thread) {
    if (thread->state != TS_RUNNING) {
        scheduler_queue(thread);
    }

    thread->state = TS_DEAD;
}

void thread_load_pc(thread_t * thread, void * pc) {
    tsr_load_pc(&thread->tsr, pc);
}

int thread_interrupt(thread_t * thread) {
    if (thread->state == TS_UNINTERRUPTABLE_WAIT) return ERROR_UNINTERRUPTABLE;

    if (thread->state == TS_INTERRUPTABLE_WAIT) {
        waiter_free(thread->waiter);

        thread->waiter = NULL;
        thread->event = NULL;
        thread->twin_thread->waiter = NULL;
        thread->twin_thread->event = NULL;

        thread->state = TS_INTERRUPTED;
        scheduler_queue(thread);
    }

    return 0;
}

__NORETURN void thread_resume(thread_t * thread) {
    switch (thread->level) {
        case TL_KERNEL: {
            resume_tsr_kernel(&thread->tsr);
        } break;

        case TL_USER: {
            signal_table_resume(thread);

            resume_tsr_user(&thread->tsr, thread->process->paging_context);
        } break;

        default: {
            panic0("Invalid thread level received in thread_resume()\n");
        } break;
    }
}

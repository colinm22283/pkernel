#include <stddef.h>

#include <process/thread.h>
#include <process/process.h>

#include <util/heap/heap.h>

#include <sys/tsr/resume_tsr.h>

thread_t * thread_create_user(pman_context_t * user_context, process_t * parent) {
    thread_t * thread = heap_alloc(sizeof(thread_t));

    thread->process = parent;

    thread->level = TL_USER;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = thread_create_kernel();
    thread->twin_thread->twin_thread = thread;

    pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, DEFAULT_THREAD_STACK_SIZE);
    thread->stack_mapping = pman_context_add_shared(user_context, PMAN_PROT_WRITE, kernel_mapping, NULL);
    pman_context_unmap(kernel_mapping);

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

thread_t * thread_create_kernel(void) {
    thread_t * thread = heap_alloc(sizeof(thread_t));

    thread->process = NULL;

    thread->level = TL_KERNEL;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = NULL;

    thread->stack_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, DEFAULT_THREAD_STACK_SIZE);

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

void thread_free(thread_t * thread) {
    if (thread->twin_thread != NULL) {
        thread->twin_thread->twin_thread = NULL;

        thread_free(thread->twin_thread);
    }

    heap_free(thread);
}

void thread_run(thread_t * thread) {
    thread->state = TS_RUNNING;
}

__NORETURN void thread_resume(thread_t * thread) {
    switch (thread->level) {
        case TL_KERNEL: {
            // resume_tsr_kernel(&thread->tsr);
        } break;

        case TL_USER: {

        } break;
    }
}
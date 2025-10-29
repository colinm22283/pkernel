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

#include "debug/vga_print.h"

thread_t * thread_create_user(pman_context_t * user_context, process_t * parent) {
    thread_t * thread = heap_alloc(sizeof(thread_t));

    thread->process = parent;

    thread->level = TL_USER;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = thread_create_kernel();
    thread->twin_thread->twin_thread = thread;
    thread->twin_thread->process = parent;

    pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, DEFAULT_THREAD_STACK_SIZE);
    thread->stack_mapping = pman_context_add_shared(user_context, PMAN_PROT_WRITE, kernel_mapping, NULL);
    pman_context_unmap(kernel_mapping);

    memset(&thread->tsr, 0, sizeof(task_state_record_t));

    tsr_set_stack(&thread->tsr, thread->stack_mapping->vaddr, thread->stack_mapping->size_pages * 0x1000);

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

thread_t * thread_create_fork(pman_context_t * user_context, process_t * parent, thread_t * target) {
    thread_t * thread = heap_alloc(sizeof(thread_t));

    thread->process = parent;

    thread->level = TL_USER;
    thread->state = TS_STOPPED;
    thread->priority = TP_MEDIUM;

    thread->twin_thread = thread_create_kernel();
    thread->twin_thread->twin_thread = thread;
    thread->twin_thread->process = parent;

    thread->stack_mapping = pman_context_get_vaddr(user_context, target->stack_mapping->vaddr);

    memcpy(&thread->tsr, &target->tsr, sizeof(task_state_record_t));

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

    tsr_set_stack(&thread->tsr, thread->stack_mapping->vaddr, thread->stack_mapping->size_pages * 0x1000);

    memset(&thread->tsr, 0, sizeof(task_state_record_t));

    thread->next = NULL;
    thread->prev = NULL;

    return thread;
}

void thread_free(thread_t * thread) {
    vga_print("FREE\n");
    if (thread->twin_thread != NULL) {
        thread->twin_thread->twin_thread = NULL;

        thread_free(thread->twin_thread);
    }

    heap_free(thread);
}

void thread_run(thread_t * thread) {
    thread->state = TS_RUNNING;

    scheduler_queue(thread);
}

void thread_load_pc(thread_t * thread, void * pc) {
    tsr_load_pc(&thread->tsr, pc);
}

__NORETURN void thread_resume(thread_t * thread) {
    switch (thread->level) {
        case TL_KERNEL: {
            resume_tsr_kernel(&thread->tsr);
        } break;

        case TL_USER: {
            // vga_print("RESUME IP: ");
            // vga_print_hex(thread->tsr.rip);
            // vga_print(" FOR ");
            // vga_print_hex(thread->process->id);
            // vga_print("\n");

            resume_tsr_user(&thread->tsr, thread->process->paging_context);
        } break;
    }
}
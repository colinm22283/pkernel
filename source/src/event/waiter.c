#include <event/waiter.h>

#include <util/memory/memcpy.h>
#include <util/heap/heap.h>

#include <sys/isr/resume_isr.h>

__NORETURN void event_waiter_resume_and_free(event_waiter_t * waiter) {
    interrupt_state_record_t isr;
    memcpy(&isr, &waiter->isr, sizeof(interrupt_state_record_t));

    pman_context_unmap(waiter->stack_mapping);
    heap_free(waiter);

    resume_isr_kernel(&isr);
}

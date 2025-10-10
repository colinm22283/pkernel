#include <stddef.h>

#include <_process/thread_table.h>
#include <_process/process.h>

#include <util/heap/heap.h>
#include <util/memory/memset.h>
#include <util/memory/memcpy.h>

#include <debug/vga_print.h>

void process_thread_table_init(process_thread_table_t * table, struct process_s * process) {
    table->process = process;

    table->current_index = 0;

    table->thread_count = 0;
    table->thread_capacity = 2;

    table->threads = heap_alloc(table->thread_capacity * sizeof(process_thread_t *));
}

void process_thread_table_free(process_thread_table_t * table) {
    for (uint64_t i = 0; i < table->thread_count; i++) {
        pman_context_unmap(table->threads[i]->stack);

        heap_free(table->threads[i]);
    }
    heap_free(table->threads);
}

process_thread_t * process_thread_table_create_thread(process_thread_table_t * table, uint64_t stack_size) {
    process_thread_t * new_thread = heap_alloc(sizeof(process_thread_t));

    table->threads[table->thread_count++] = new_thread;

    new_thread->state = TS_RUNNING;

    memset(&new_thread->isr, 0, sizeof(interrupt_state_record_t));

    pman_mapping_t * temp_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, stack_size);
    new_thread->stack = pman_context_add_shared(table->process->paging_context, PMAN_PROT_WRITE, temp_mapping, NULL);
    pman_context_unmap(temp_mapping);

    new_thread->isr.rip = (uint64_t) table->process->text->vaddr;
    new_thread->isr.rsp = ((intptr_t) new_thread->stack->vaddr + new_thread->stack->size_pages * 0x1000 - 1) & ~0b111;

    if (table->thread_count == table->thread_capacity) {
        table->thread_capacity *= 2;

        table->threads = heap_realloc(table->threads, table->thread_capacity * sizeof(process_thread_t *));
    }

    return new_thread;
}

process_thread_t * process_thread_table_create_thread_fork(process_thread_table_t * table, __MAYBE_UNUSED process_t * parent_process, process_thread_t * parent_thread) {
    process_thread_t * new_thread = heap_alloc(sizeof(process_thread_t));

    table->threads[table->thread_count++] = new_thread;

    new_thread->state = TS_RUNNING;

    memcpy(&new_thread->isr, &parent_thread->isr, sizeof(interrupt_state_record_t));

    new_thread->stack = pman_context_add_borrowed(table->process->paging_context, PMAN_PROT_WRITE, parent_thread->stack, parent_thread->stack->vaddr);

    if (table->thread_count == table->thread_capacity) {
        table->thread_capacity *= 2;

        table->threads = heap_realloc(table->threads, table->thread_capacity * sizeof(process_thread_t *));
    }

    return new_thread;
}

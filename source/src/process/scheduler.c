#include <stddef.h>

#include <process/scheduler.h>

#include <timer/timer.h>

#include <interface/interface_map.h>

#include <task/enter_user_mode.h>

#include <memory/gdt.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

#include <sys/asm/sti.h>
#include <sys/asm/cli.h>
#include <sys/asm/hlt.h>

#include <debug/vga_print.h>

process_t * volatile current_process;

process_id_t scheduler_current_id;
process_t scheduler_head, scheduler_tail;

void scheduler_timer_handler() {
    if (current_process->next != &scheduler_tail && current_process != &scheduler_tail) current_process = current_process->next;
    else current_process = scheduler_head.next;
}

void scheduler_init(void) {
    scheduler_current_id = 0;

    scheduler_head.next = &scheduler_tail;
    scheduler_head.prev = NULL;

    scheduler_tail.next = NULL;
    scheduler_tail.prev = &scheduler_head;

    current_process = scheduler_head.next;

    timer_init(scheduler_timer_handler, NULL, 0, 1);
}

process_t * scheduler_queue(
    process_id_t parent_id,
    uint64_t text_size,
    uint64_t data_size,
    uint64_t rodata_size,
    uint64_t bss_size,
    uint64_t stack_size
) {
    process_t * process = heap_alloc(sizeof(process_t));

    process_init(process, parent_id, scheduler_current_id++, text_size, data_size, rodata_size, bss_size, stack_size);

    process->next = scheduler_head.next;
    process->prev = &scheduler_head;

    scheduler_head.next->prev = process;
    scheduler_head.next = process;

    if (current_process == &scheduler_tail) current_process = scheduler_head.next;

    return process;
}

process_t * scheduler_queue_fork(
    process_t * parent
) {
    process_t * process = heap_alloc(sizeof(process_t));

    process_init_fork(process, parent, scheduler_current_id++);

    process->next = scheduler_head.next;
    process->prev = &scheduler_head;

    scheduler_head.next->prev = process;
    scheduler_head.next = process;

    if (current_process == &scheduler_tail) current_process = scheduler_head.next;

    return process;
}

process_t * scheduler_get_id(process_id_t id) {
    process_t * process = scheduler_head.next;

    while (process != &scheduler_tail) {
        if (process->id == id) return process;

        process = process->next;
    }

    return NULL;
}

__NORETURN void scheduler_start(void) {
    while (true) {
        if (current_process != &scheduler_tail) {
            switch (current_process->state) {
                case PS_RUNNING: {
                    signal_table_enter(&current_process->signal_table, current_process);

                    current_process->thread_table.current_index++;
                    if (current_process->thread_table.current_index >= current_process->thread_table.thread_count) current_process->thread_table.current_index = 0;

                    if (current_process->thread_table.threads[current_process->thread_table.current_index]->state == TS_RUNNING) {
                        static interrupt_state_record_t static_isr;

                        memcpy(&static_isr, &current_process->thread_table.threads[current_process->thread_table.current_index]->isr, sizeof(interrupt_state_record_t));

                        enter_user_mode(
                            GDT_USER_CODE,
                            GDT_USER_DATA,
                            current_process->paging_context->top_level_table_paddr,
                            &static_isr
                        );
                    }

                    current_process = current_process->next;
                } break;

                case PS_STOPPED: {
                    current_process = current_process->next;
                } break;

                case PS_DEAD: {
                    /* process_t * parent = scheduler_get_id(current_process->parent_id); */
                    /* if (parent != NULL) { */
                        /* signal_table_invoke(&parent->signal_table, SIGNAL_CHILD, (void *) current_process->id); */
                    /* } */

                    current_process->prev->next = current_process->next;
                    current_process->next->prev = current_process->prev;

                    process_t * next = current_process->next;

                    process_free(current_process);
                    heap_free(current_process);

                    current_process = next;
                } break;

                default: break;
            }
        }
        else {
            current_process = scheduler_head.next;
        }

        // sti();
        // while (true) hlt();
    }
}

process_t * scheduler_current_process(void) {
    return current_process;
}

process_thread_t * scheduler_current_thread(void) {
    return current_process->thread_table.threads[current_process->thread_table.current_index];
}

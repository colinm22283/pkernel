#include <stddef.h>

#include <paging/manager.h>
#include <paging/kernel_translation.h>

#include <scheduler/scheduler.h>

#include <interrupt/interrupt_registry.h>

#include <device/device.h>

#include <util/heap/heap.h>
#include <util/math/div_up.h>
#include <util/memory/memset.h>
#include <util/memory/memcpy.h>
#include <util/string/strlen.h>

#include <pkos/defs.h>

#include <sys/paging/page_type.h>
#include <sys/paging/load_page_table.h>
#include <sys/paging/read_fault_vaddr.h>
#include <sys/halt.h>

#include <debug/vga_print.h>
#include <util/heap/internal.h>

#include <pkos/syscalls.h>

pman_context_t kernel_context;

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * isr, void * _error_code);

void pman_init(void) {
    kernel_context.top_level_table_allocation.vaddr = NULL;
    kernel_context.top_level_table = &paging_kernel_pml4t;
    kernel_context.top_level_table_paddr = paging_kernel_virtual_to_physical(paging_kernel_pml4t);

    valloc_init(&kernel_context.valloc);

    valloc_reserve(&kernel_context.valloc, (void *) 0, (uint64_t) KERNEL_END);

    kernel_context.head.next = &kernel_context.tail;
    kernel_context.head.prev = NULL;
    kernel_context.tail.next = NULL;
    kernel_context.tail.prev = &kernel_context.head;

    interrupt_registry_register(IC_PAGE_FAULT, pman_page_fault_handler);
}

pman_context_t * pman_new_context(void) {
    pman_context_t * context = heap_alloc(sizeof(pman_context_t));

    if (!paging_talloc_alloc(&context->top_level_table_allocation)) return NULL;

    context->top_level_table = context->top_level_table_allocation.vaddr;
    context->top_level_table_paddr = context->top_level_table_allocation.paddr;

    memset(context->top_level_table, 0, sizeof(pml4t64_t));
    pml4t64_entry_t * pml4t_entry = pml4t64_map_address(context->top_level_table, paging_kernel_virtual_to_physical(paging_kernel_pdpt), KERNEL_START);
    pml4t_entry->present = 1;
    pml4t_entry->read_write = 1;
    pml4t_entry->user_super = 1;

    valloc_init(&context->valloc);

    valloc_reserve(&context->valloc, (void *) 0, DIV_UP((uint64_t) KERNEL_END, 0x8000000000) * 0x8000000000);

    context->head.next = &context->tail;
    context->head.prev = NULL;
    context->tail.next = NULL;
    context->tail.prev = &context->head;

    return context;
}

pman_context_t * pman_new_kernel_context(void) {
    pman_context_t * context = heap_alloc(sizeof(pman_context_t));

    if (!paging_talloc_alloc(&context->top_level_table_allocation)) return NULL;

    context->top_level_table = context->top_level_table_allocation.vaddr;
    context->top_level_table_paddr = context->top_level_table_allocation.paddr;

    memset(context->top_level_table, 0, sizeof(pml4t64_t));
    pml4t64_entry_t * pml4t_entry = pml4t64_map_address(context->top_level_table, paging_kernel_virtual_to_physical(paging_kernel_pdpt), KERNEL_START);
    pml4t_entry->present = 1;
    pml4t_entry->read_write = 0;
    pml4t_entry->user_super = 1;

    valloc_init(&context->valloc);

    valloc_reserve(&context->valloc, (void *) 0, (uint64_t) KERNEL_END);

    context->head.next = &context->tail;
    context->head.prev = NULL;
    context->tail.next = NULL;
    context->tail.prev = &context->head;

    return context;
}

error_number_t pman_free_context(pman_context_t * context) { // TODO
    while (context->head.next != &context->tail) {
        pman_context_unmap(context->head.next);
    }

    valloc_free(&context->valloc);

    paging_talloc_free(&context->top_level_table_allocation);

    heap_free(context);

    return ERROR_OK;
}

void pman_context_load_table(pman_context_t * context) {
    load_page_table((void *) context->top_level_table_paddr);
}

pman_mapping_t * pman_context_add_alloc(pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t size) {
    uint64_t size_pages = DIV_UP(size, 0x1000);

    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;
    mapping->type = PMAN_MAPPING_ALLOC;
    mapping->protection = prot;
    mapping->size_pages = size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, size);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, size);

    mapping->alloc.references = 1;

    palloc_alloc(&mapping->alloc.palloc, size_pages * 0x1000);

    mapping->alloc.mapping_count = mapping->alloc.palloc.paddr_count;
    mapping->alloc.mappings = heap_alloc(mapping->alloc.palloc.paddr_count * sizeof(paging_mapping_t));

    page_data_t * current_vaddr = mapping->vaddr;

    for (uint64_t i = 0; i < mapping->alloc.palloc.paddr_count; i++) {
        paging_map_ex(
            context->top_level_table,
            &mapping->alloc.mappings[i],
            mapping->alloc.palloc.paddrs[i],
            current_vaddr,
            1,
            prot & PMAN_PROT_WRITE,
            !(prot & PMAN_PROT_EXECUTE),
            context != pman_kernel_context()
        );

        current_vaddr++;
    }

    mapping->prev = &context->head;
    mapping->next = context->head.next;

    context->head.next->prev = mapping;
    context->head.next = mapping;

    return mapping;
}

pman_mapping_t * pman_context_add_map(pman_context_t * context, pman_protection_flags_t prot, void * vaddr, uint64_t paddr, uint64_t size) {
    uint64_t size_pages = DIV_UP(size, 0x1000);

    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;
    mapping->type = PMAN_MAPPING_MAP;
    mapping->protection = prot;
    mapping->size_pages = size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, size);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, size);

    mapping->map.references = 1;

    palloc_alloc_contiguous(&mapping->map.palloc, size_pages * 0x1000, paddr);

    paging_map_ex(
        context->top_level_table,
        &mapping->map.mapping,
        mapping->map.palloc.paddrs[0],
        mapping->vaddr,
        size_pages,
        prot & PMAN_PROT_WRITE,
        !(prot & PMAN_PROT_EXECUTE),
        context != pman_kernel_context()
    );

    mapping->prev = &context->head;
    mapping->next = context->head.next;

    context->head.next->prev = mapping;
    context->head.next = mapping;

    return mapping;
}

pman_mapping_t * pman_context_add_borrowed(pman_context_t * context, pman_protection_flags_t prot, pman_mapping_t * lender, void * vaddr) {
    pman_mapping_t * root_lender = get_root_mapping(lender);

    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;
    mapping->type = PMAN_MAPPING_BORROWED;
    mapping->protection = prot;
    mapping->size_pages = lender->size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, lender->size_pages * 0x1000);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, lender->size_pages * 0x1000);

    mapping->borrowed.lender = root_lender;

    if (root_lender->type == PMAN_MAPPING_ALLOC) {
        root_lender->alloc.references++;

        mapping->borrowed.mapping_count = root_lender->alloc.mapping_count;
        mapping->borrowed.mappings = heap_alloc(mapping->borrowed.mapping_count * sizeof(paging_mapping_t));

        page_data_t * current_vaddr = mapping->vaddr;

        for (uint64_t i = 0; i < mapping->borrowed.mapping_count; i++) {
            paging_map_ex(
                context->top_level_table,
                &mapping->borrowed.mappings[i],
                root_lender->alloc.palloc.paddrs[i],
                current_vaddr,
                1,
                false,
                !(prot & PMAN_PROT_EXECUTE),
                context != pman_kernel_context()
            );

            current_vaddr++;
        }
    }
    else {
        root_lender->map.references++;

        mapping->borrowed.mapping_count = 1;
        mapping->borrowed.mappings = heap_alloc(mapping->borrowed.mapping_count * sizeof(paging_mapping_t));

        paging_map_ex(
            context->top_level_table,
            &mapping->borrowed.mappings[0],
            root_lender->map.palloc.paddrs[0],
            mapping->vaddr,
            root_lender->size_pages,
            false,
            !(prot & PMAN_PROT_EXECUTE),
            context != pman_kernel_context()
        );
    }

    mapping->prev = &context->head;
    mapping->next = context->head.next;

    context->head.next->prev = mapping;
    context->head.next = mapping;

    return mapping;
}

pman_mapping_t * pman_context_add_shared(pman_context_t * context, pman_protection_flags_t prot, pman_mapping_t * lender, void * vaddr) {
    pman_mapping_t * root_lender = get_root_mapping(lender);

    pman_mapping_t * mapping = heap_alloc(sizeof(pman_mapping_t));

    mapping->context = context;
    mapping->type = PMAN_MAPPING_SHARED;
    mapping->protection = prot;
    mapping->size_pages = lender->size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, lender->size_pages * 0x1000);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, lender->size_pages * 0x1000);

    mapping->shared.lender = root_lender;

    if (root_lender->type == PMAN_MAPPING_ALLOC) {
        root_lender->alloc.references++;

        mapping->shared.mapping_count = root_lender->alloc.mapping_count;
        mapping->shared.mappings = heap_alloc(mapping->shared.mapping_count * sizeof(paging_mapping_t));

        page_data_t * current_vaddr = mapping->vaddr;

        for (uint64_t i = 0; i < mapping->shared.mapping_count; i++) {
            paging_map_ex(
                context->top_level_table,
                &mapping->shared.mappings[i],
                root_lender->alloc.palloc.paddrs[i],
                current_vaddr,
                1,
                prot & PMAN_PROT_WRITE,
                !(prot & PMAN_PROT_EXECUTE),
                context != pman_kernel_context()
            );

            current_vaddr++;
        }
    }
    else {
        root_lender->map.references++;

        mapping->shared.mapping_count = 1;
        mapping->shared.mappings = heap_alloc(mapping->shared.mapping_count * sizeof(paging_mapping_t));

        paging_map_ex(
            context->top_level_table,
            &mapping->shared.mappings[0],
            root_lender->map.palloc.paddrs[0],
            mapping->vaddr,
            root_lender->size_pages,
            prot & PMAN_PROT_WRITE,
            !(prot & PMAN_PROT_EXECUTE),
            context != pman_kernel_context()
        );
    }

    mapping->prev = &context->head;
    mapping->next = context->head.next;

    context->head.next->prev = mapping;
    context->head.next = mapping;

    return mapping;
}

error_number_t pman_context_unmap(pman_mapping_t * mapping) {
    mapping->next->prev = mapping->prev;
    mapping->prev->next = mapping->next;

    switch (mapping->type) {
        case PMAN_MAPPING_ALLOC: {
            mapping->alloc.references--;

            if (mapping->alloc.references == 0) {
                valloc_release(&mapping->context->valloc, mapping->vaddr);

                for (uint64_t i = 0; i < mapping->alloc.mapping_count; i++) paging_unmap(mapping->context->top_level_table, &mapping->alloc.mappings[i]);
                heap_free(mapping->alloc.mappings);

                palloc_free(&mapping->alloc.palloc);

                heap_free(mapping);
            }
        } break;

        case PMAN_MAPPING_MAP: {
            mapping->map.references--;
            if (mapping->map.references == 0) {
                valloc_release(&mapping->context->valloc, mapping->vaddr);

                paging_unmap(mapping->context->top_level_table, &mapping->map.mapping);

                palloc_free(&mapping->map.palloc);

                heap_free(mapping);
            }
        } break;

        case PMAN_MAPPING_BORROWED: {
            pman_context_unmap(mapping->borrowed.lender);

            valloc_release(&mapping->context->valloc, mapping->vaddr);

            for (uint64_t i = 0; i < mapping->borrowed.mapping_count; i++) paging_unmap(mapping->context->top_level_table, &mapping->borrowed.mappings[i]);
            heap_free(mapping->borrowed.mappings);

            heap_free(mapping);
        } break;

        case PMAN_MAPPING_SHARED: {
            pman_context_unmap(mapping->shared.lender);

            valloc_release(&mapping->context->valloc, mapping->vaddr);

            for (uint64_t i = 0; i < mapping->shared.mapping_count; i++) paging_unmap(mapping->context->top_level_table, &mapping->shared.mappings[i]);
            heap_free(mapping->shared.mappings);

            heap_free(mapping);
        } break;

        default: break;
    }

    return ERROR_OK;
}

pman_mapping_t * pman_context_resize(pman_mapping_t * mapping, uint64_t size) {
    switch (mapping->type) {
        case PMAN_MAPPING_ALLOC: {
            pman_mapping_t * new_alloc = pman_context_add_alloc(
                pman_kernel_context(),
                mapping->protection,
                NULL,
                size
            );

            memcpy(new_alloc->vaddr, mapping->vaddr, mapping->size_pages * 0x1000);

            pman_context_unmap(mapping);

            return new_alloc;
        } break;

        case PMAN_MAPPING_BORROWED: {
            pman_context_t * context = mapping->context;
            pman_protection_flags_t prot = mapping->protection;
            void * vaddr = mapping->vaddr;

            pman_mapping_t * new_lender = pman_context_add_alloc(
                pman_kernel_context(),
                mapping->borrowed.lender->protection,
                NULL,
                size
            );

            memcpy(new_lender->vaddr, mapping->borrowed.lender->vaddr, mapping->size_pages * 0x1000);

            pman_context_unmap(mapping);

            pman_mapping_t * new_borrow = pman_context_add_borrowed(
                context,
                prot,
                new_lender,
                vaddr
            );

            pman_context_unmap(new_lender);

            return new_borrow;
        } break;

        // TODO: add cases
        default: {
            vga_print("bad\n");
        } break;
    }

    return NULL;
}

pman_mapping_t * pman_context_get_vaddr(pman_context_t * context, void * vaddr) {
    pman_mapping_t * mapping = context->head.next;

    while (mapping != &context->tail) {
        if (
            vaddr >= mapping->vaddr &&
            vaddr < (void *) ((char *) mapping->vaddr + mapping->size_pages * 0x1000)
        ) return mapping;

        mapping = mapping->next;
    }

    return NULL;
}

pman_mapping_t * pman_context_prepare_write(process_t * process, pman_mapping_t * mapping) {
    if (mapping->type == PMAN_MAPPING_BORROWED) {
        // TODO
        // pman_protection_flags_t mapping_protection = mapping->protection;
        // void * mapping_vaddr = mapping->vaddr;
        // pman_context_t * context = mapping->context;
        //
        // pman_mapping_t ** matching_mapping = NULL;
        // if (process->text == mapping) matching_mapping = &process->text;
        // if (process->data == mapping) matching_mapping = &process->data;
        // if (process->rodata == mapping) matching_mapping = &process->rodata;
        // if (process->bss == mapping) matching_mapping = &process->bss;
        // for (uint64_t i = 0; i < process->thread_table.thread_count; i++) {
        //     if (process->thread_table.threads[i]->stack == mapping) {
        //         matching_mapping = &process->thread_table.threads[i]->stack;
        //     }
        // }
        //
        // pman_mapping_t * root_mapping = get_root_mapping(mapping);
        //
        // pman_mapping_t * kernel_alloc = pman_context_add_alloc(
        //     pman_kernel_context(),
        //     PMAN_PROT_WRITE,
        //     NULL,
        //     root_mapping->size_pages * 0x1000
        // );
        //
        // memcpy(kernel_alloc->vaddr, root_mapping->vaddr, root_mapping->size_pages * 0x1000);
        //
        // pman_context_unmap(mapping);
        //
        // pman_mapping_t * user_mapping = pman_context_add_shared(
        //     context,
        //     mapping_protection,
        //     kernel_alloc,
        //     mapping_vaddr
        // );
        //
        // pman_context_unmap(kernel_alloc);
        //
        // if (matching_mapping != NULL) {
        //     *matching_mapping = user_mapping;
        // }

        // return user_mapping;
        return NULL;
    }
    else return mapping;
}

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * isr, void * _error_code) {
    halt();

    // page_fault_error_code_t * error_code = (page_fault_error_code_t *) _error_code;
    //
    // heap_check();
    //
    // process_t * current_process = scheduler_current_process();
    //
    // pman_context_t * current_context;
    //
    // if (error_code->user) current_context = current_process->paging_context;
    // else current_context = pman_kernel_context();
    //
    // void * fault_vaddr = read_fault_vaddr();
    //
    // pman_mapping_t * mapping = pman_context_get_vaddr(current_context, fault_vaddr);
    //
    // if (mapping == NULL) {
    //     fs_file_t * out_file = process_file_table_get(&current_process->file_table, stdout);
    //
    //     if (out_file != NULL) file_write(out_file, "PAGE FAULT: Bad address\n", 24);
    //
    //     if (error_code->present) vga_print("Reason: PROTECTION VIOLATION\n");
    //     else vga_print("Reason: NOT PRESENT\n");
    //     if (error_code->write) vga_print("WRITE\n");
    //     if (error_code->instruction_fetch) vga_print("INSTRUCTION FETCH\n");
    //     if (error_code->user) vga_print("USER\n");
    //
    //     vga_print("Fault VAddr: ");
    //     vga_print_hex((uint64_t) fault_vaddr);
    //     vga_print("\n");
    //
    //     vga_print("Proc RSP: ");
    //     vga_print_hex(tsr->rsp);
    //     vga_print("\n");
    //
    //     process_kill(current_process);
    //     return;
    // }
    //
    // if (pman_context_prepare_write(scheduler_current_process(), mapping) == NULL) {
    //     fs_file_t * out_file = process_file_table_get(&current_process->file_table, stdout);
    //
    //     if (out_file != NULL) file_write(out_file, "PAGE FAULT: Unwritable\n", 23);
    //
    //     process_kill(current_process);
    //     return;
    // }
    //
    // if (!error_code->user) halt();
}

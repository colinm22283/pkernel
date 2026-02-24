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
#include <sys/paging/page_size.h>
#include <sys/paging/load_page_table.h>
#include <sys/paging/read_fault_vaddr.h>
#include <sys/halt.h>
#include <sys/panic.h>

#include <util/heap/internal.h>

#include <pkos/syscalls.h>

#include <config/pman.h>

#ifdef PMAN_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("paging manager");

pman_context_t kernel_context;
pman_mapping_t * kernel_mapping;

pml4t64_t kern_pml4t;

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * isr, void * _error_code);

void pman_init(void) {
    kernel_context.top_level_table_allocation.vaddr = NULL;
    kernel_context.top_level_table = &kern_pml4t;
    kernel_context.top_level_table_paddr = paging_kernel_virtual_to_physical(kern_pml4t);

    valloc_init(&kernel_context.valloc);

    // valloc_reserve(&kernel_context.valloc, (void *) 0, (uint64_t) KERNEL_END);

    kernel_context.head.next = &kernel_context.tail;
    kernel_context.head.prev = NULL;
    kernel_context.tail.next = NULL;
    kernel_context.tail.prev = &kernel_context.head;

    interrupt_registry_register(IC_PAGE_FAULT, pman_page_fault_handler);

    kernel_mapping = pman_context_add_map(pman_kernel_context(), PMAN_PROT_WRITE | PMAN_PROT_EXECUTE, KERNEL_START, primary_region_start, KERNEL_SIZE);

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            &kern_pml4t,
            paging_kernel_virtual_to_physical(paging_bitmap_pdpt),
            PAGING_BITMAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            &kern_pml4t,
            paging_kernel_virtual_to_physical(paging_tmap_pdpt),
            PAGING_TMAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            &kern_pml4t,
            paging_kernel_virtual_to_physical(paging_talloc_pdpt),
            PAGING_TALLOC_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    pman_context_load_table(pman_kernel_context());
}

pman_context_t * pman_new_context(void) {
    kprintf("New user context");

    pman_context_t * context = heap_alloc_debug(sizeof(pman_context_t), "user pman_context_t");

    if (!paging_talloc_alloc(&context->top_level_table_allocation)) return NULL;

    context->top_level_table = context->top_level_table_allocation.vaddr;
    context->top_level_table_paddr = context->top_level_table_allocation.paddr;

    memset(context->top_level_table, 0, sizeof(pml4t64_t));
    // pml4t64_entry_t * pml4t_entry = pml4t64_map_address(context->top_level_table, paging_kernel_virtual_to_physical(paging_kernel_pdpt), KERNEL_START);
    // pml4t_entry->present = 1;
    // pml4t_entry->read_write = 1;
    // pml4t_entry->user_super = 1;

    valloc_init(&context->valloc);

    // valloc_reserve(&context->valloc, (void *) 0, PAGE_SIZE);
    // void * kernel_addr = valloc_reserve(&context->valloc, (void *) KERNEL_START, (uint64_t) (KERNEL_END - KERNEL_START));

    context->head.next = &context->tail;
    context->head.prev = NULL;
    context->tail.next = NULL;
    context->tail.prev = &context->head;

    kprintf("oh yeah %i", KERNEL_SIZE);
    if (pman_context_add_shared(context, PMAN_PROT_EXECUTE | PMAN_PROT_WRITE, kernel_mapping, KERNEL_START) == NULL) {
        panic0("Oh golly\n");
    }
    kprintf("yep");

    return context;
}

pman_context_t * pman_new_kernel_context(void) {
    kprintf("New kernel context");

    return pman_kernel_context();

    // pman_context_t * context = heap_alloc_debug(sizeof(pman_context_t), "kernel pman_context_t");
    //
    // if (!paging_talloc_alloc(&context->top_level_table_allocation)) return NULL;
    //
    // context->top_level_table = context->top_level_table_allocation.vaddr;
    // context->top_level_table_paddr = context->top_level_table_allocation.paddr;
    //
    // memset(context->top_level_table, 0, sizeof(pml4t64_t));
    // // pml4t64_entry_t * pml4t_entry = pml4t64_map_address(context->top_level_table, paging_kernel_virtual_to_physical(paging_kernel_pdpt), KERNEL_START);
    // // pml4t_entry->present = 1;
    // // pml4t_entry->read_write = 0;
    // // pml4t_entry->user_super = 1;
    //
    // valloc_init(&context->valloc);
    //
    // // valloc_reserve(&context->valloc, (void *) 0, (uint64_t) KERNEL_END);
    //
    // context->head.next = &context->tail;
    // context->head.prev = NULL;
    // context->tail.next = NULL;
    // context->tail.prev = &context->head;
    //
    // kprintf("oh yeah");
    // if (pman_context_add_shared(context, PMAN_PROT_EXECUTE | PMAN_PROT_WRITE, kernel_mapping, KERNEL_START) == NULL) {
    //     panic0("Oh golly\n");
    // }
    // kprintf("yep");
    //
    // return context;
}

error_number_t pman_free_context(pman_context_t * context) { // TODO
    kprintf("Free context");

    if (context == pman_kernel_context()) return ERROR_OK;

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
    kprintf("Add allocation");

    uint64_t size_pages = DIV_UP(size, PAGE_SIZE);

    pman_mapping_t * mapping = heap_alloc_debug(sizeof(pman_mapping_t), "alloc mapping");

    mapping->context = context;
    mapping->type = PMAN_MAPPING_ALLOC;
    mapping->protection = prot;
    mapping->size_pages = size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, size);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, size);

    mapping->alloc.references = 1;

    palloc_alloc(&mapping->alloc.palloc, size_pages * PAGE_SIZE);

    mapping->alloc.mapping_count = mapping->alloc.palloc.paddr_count;
    mapping->alloc.mappings = heap_alloc_debug(mapping->alloc.palloc.paddr_count * sizeof(paging_mapping_t), "alloc mapping mappings");

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
    kprintf("Add map");

    uint64_t size_pages = DIV_UP(size, PAGE_SIZE);

    pman_mapping_t * mapping = heap_alloc_debug(sizeof(pman_mapping_t), "map mapping");

    mapping->context = context;
    mapping->type = PMAN_MAPPING_MAP;
    mapping->protection = prot;
    mapping->size_pages = size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, size);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, size);

    mapping->map.references = 1;

    palloc_alloc_contiguous(&mapping->map.palloc, size_pages * PAGE_SIZE, paddr);

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
    kprintf("Add borrow");

    pman_mapping_t * root_lender = get_root_mapping(lender);

    pman_mapping_t * mapping = heap_alloc_debug(sizeof(pman_mapping_t), "borrow mapping");

    mapping->context = context;
    mapping->type = PMAN_MAPPING_BORROWED;
    mapping->protection = prot;
    mapping->size_pages = lender->size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, lender->size_pages * PAGE_SIZE);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, lender->size_pages * PAGE_SIZE);

    mapping->borrowed.lender = root_lender;

    if (root_lender->type == PMAN_MAPPING_ALLOC) {
        root_lender->alloc.references++;

        mapping->borrowed.mapping_count = root_lender->alloc.mapping_count;
        mapping->borrowed.mappings = heap_alloc_debug(mapping->borrowed.mapping_count * sizeof(paging_mapping_t), "borrow mapping alloc mappings");

        page_data_t * current_vaddr = mapping->vaddr;

        if (mapping->borrowed.mapping_count == 0) {
            panic0("yep");
        }

        for (uint64_t i = 0; i < mapping->borrowed.mapping_count; i++) {
            if ((intptr_t) current_vaddr < 0xD0000000) {
                debug_print("gurp\n");
                halt();
            }

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
        mapping->borrowed.mappings = heap_alloc_debug(mapping->borrowed.mapping_count * sizeof(paging_mapping_t), "borrow mapping map mappings");

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
    kprintf("Add share");

    pman_mapping_t * root_lender = get_root_mapping(lender);

    pman_mapping_t * mapping = heap_alloc_debug(sizeof(pman_mapping_t), "shared mapping");

    mapping->context = context;
    mapping->type = PMAN_MAPPING_SHARED;
    mapping->protection = prot;
    mapping->size_pages = lender->size_pages;

    if (vaddr == NULL) mapping->vaddr = valloc_alloc(&context->valloc, lender->size_pages * PAGE_SIZE);
    else mapping->vaddr = valloc_reserve(&context->valloc, vaddr, lender->size_pages * PAGE_SIZE);

    if (mapping->vaddr == NULL) return NULL;

    mapping->shared.lender = root_lender;

    if (root_lender->type == PMAN_MAPPING_ALLOC) {
        root_lender->alloc.references++;

        mapping->shared.mapping_count = root_lender->alloc.mapping_count;
        mapping->shared.mappings = heap_alloc_debug(mapping->shared.mapping_count * sizeof(paging_mapping_t), "shared mapping alloc mappings");

        page_data_t * current_vaddr = mapping->vaddr;

        for (uint64_t i = 0; i < mapping->shared.mapping_count; i++) {
            kprintf("Mapping %p -> %p", current_vaddr, (void *) root_lender->alloc.palloc.paddrs[i]);

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
        kprintf("Mapping %p -> %p", mapping->vaddr, (void *) root_lender->alloc.palloc.paddrs[0]);

        root_lender->map.references++;

        mapping->shared.mapping_count = 1;
        mapping->shared.mappings = heap_alloc_debug(mapping->shared.mapping_count * sizeof(paging_mapping_t), "shared mapping map mappings");

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
    kprintf("Unmap");

    switch (mapping->type) {
        case PMAN_MAPPING_ALLOC: {
            mapping->alloc.references--;

            if (mapping->alloc.references == 0) {
                mapping->next->prev = mapping->prev;
                mapping->prev->next = mapping->next;

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
                mapping->next->prev = mapping->prev;
                mapping->prev->next = mapping->next;

                valloc_release(&mapping->context->valloc, mapping->vaddr);

                paging_unmap(mapping->context->top_level_table, &mapping->map.mapping);

                palloc_free(&mapping->map.palloc);

                heap_free(mapping);
            }
        } break;

        case PMAN_MAPPING_BORROWED: {
            mapping->next->prev = mapping->prev;
            mapping->prev->next = mapping->next;

            pman_context_unmap(mapping->borrowed.lender);

            valloc_release(&mapping->context->valloc, mapping->vaddr);

            for (uint64_t i = 0; i < mapping->borrowed.mapping_count; i++) paging_unmap(mapping->context->top_level_table, &mapping->borrowed.mappings[i]);

            heap_free(mapping->borrowed.mappings);

            heap_free(mapping);
        } break;

        case PMAN_MAPPING_SHARED: {
            mapping->next->prev = mapping->prev;
            mapping->prev->next = mapping->next;

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
    kprintf("Resize mapping");

    switch (mapping->type) {
        case PMAN_MAPPING_ALLOC: {
            pman_mapping_t * new_alloc = pman_context_add_alloc(
                pman_kernel_context(),
                mapping->protection,
                NULL,
                size
            );

            memcpy(new_alloc->vaddr, mapping->vaddr, mapping->size_pages * PAGE_SIZE);

            pman_context_unmap(mapping);

            return new_alloc;
        } break;

        case PMAN_MAPPING_BORROWED: {
            pman_context_t * context = mapping->context;
            pman_protection_flags_t prot = mapping->protection;
            void * vaddr = mapping->vaddr;

            pman_mapping_t * new_lender = pman_context_add_alloc(
                pman_kernel_context(),
                PMAN_PROT_WRITE,
                NULL,
                size
            );

            memcpy(new_lender->vaddr, get_root_mapping(mapping)->vaddr, mapping->size_pages * 0x1000);

            pman_context_unmap(mapping);

            pman_mapping_t * new_mapping = pman_context_add_shared(
                context,
                prot,
                new_lender,
                vaddr
            );

            pman_context_unmap(new_lender);

            return new_mapping;
        } break;

        // TODO: add cases
        default: {
            debug_print("bad\n");
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
    kprintf("Prepare write");

    if (mapping->type == PMAN_MAPPING_BORROWED) {
        pman_protection_flags_t mapping_protection = mapping->protection;
        void * mapping_vaddr = mapping->vaddr;
        pman_context_t * context = mapping->context;

        pman_mapping_t * root_mapping = get_root_mapping(mapping);

        if (root_mapping->alloc.references == 1) {
            pman_add_reference(root_mapping);

            pman_context_unmap(mapping);

            pman_mapping_t * user_mapping = pman_context_add_shared(
                context,
                mapping_protection,
                root_mapping,
                mapping_vaddr
            );

            pman_context_unmap(root_mapping);

            process_remap(process, mapping, user_mapping);

            return user_mapping;
        }
        else {
            pman_mapping_t * kernel_alloc = pman_context_add_alloc(
                pman_kernel_context(),
                PMAN_PROT_WRITE,
                NULL,
                root_mapping->size_pages * 0x1000
            );

            memcpy(kernel_alloc->vaddr, root_mapping->vaddr, root_mapping->size_pages * 0x1000);

            pman_context_unmap(mapping);

            pman_mapping_t * user_mapping = pman_context_add_shared(
                context,
                mapping_protection,
                kernel_alloc,
                mapping_vaddr
            );

            pman_context_unmap(kernel_alloc);

            process_remap(process, mapping, user_mapping);

            return user_mapping;
        }
    }
    else return mapping;
}

bool looping = false;

void pman_page_fault_handler(interrupt_code_t channel, task_state_record_t * tsr, void * _error_code) {
    page_fault_error_code_t * error_code = (page_fault_error_code_t *) _error_code;

    if (!error_code->user) {
        if (looping) {
            panic0("Stopped kernel page fault loop");
        }
        else {
            looping = true;
        }
    }

    void * fault_vaddr = read_fault_vaddr();

    process_t * current_process = scheduler_current_process();

    kprintf(
        "Page fault caught for access to %p while executing at %p from process %i",
        (void *) fault_vaddr,
        (void *) tsr->rip,
        current_process->id
    );

    // heap_check();

    pman_context_t * current_context;

    if (error_code->user) current_context = current_process->paging_context;
    else {
        debug_print("PAGE FAULT OCCURRED IN KERNEL\n");
        debug_print("vaddr: ");
        debug_print_hex((uint64_t) fault_vaddr);
        debug_print("\n");

        debug_print("ip: ");
        debug_print_hex(tsr->rip);
        debug_print("\n");

        if (error_code->present) debug_print("Reason: PROTECTION VIOLATION\n");
        else debug_print("Reason: NOT PRESENT\n");
        if (error_code->write) debug_print("WRITE\n");
        if (error_code->instruction_fetch) debug_print("INSTRUCTION FETCH\n");
        if (error_code->user) debug_print("USER\n");

        halt();
    }

    pman_mapping_t * mapping = pman_context_get_vaddr(current_context, fault_vaddr);

    if (mapping == NULL) {
        // fs_file_t * out_file = file_table_get(&current_process->file_table, stdout);

        // if (out_file != NULL) file_write(out_file, "PAGE FAULT: Bad address\n", 24);

        debug_print("Fault VAddr: ");
        debug_print_hex((uint64_t) fault_vaddr);
        debug_print("\n");

        debug_print("Fault IP: ");
        debug_print_hex(tsr->rip);
        debug_print("\n");

        if (error_code->present) debug_print("Reason: PROTECTION VIOLATION\n");
        else debug_print("Reason: NOT PRESENT\n");
        if (error_code->write) debug_print("WRITE\n");
        if (error_code->instruction_fetch) debug_print("INSTRUCTION FETCH\n");
        if (error_code->user) debug_print("USER\n");

        thread_t * current_thread = scheduler_current_thread();

        signal_table_invoke(current_process, SIG_PAGE);

        thread_resume(current_thread);

        // process_kill(current_process);
        return;
    }

    if (pman_context_prepare_write(current_process, mapping) == NULL) {
        fs_file_t * out_file = file_table_get(&current_process->file_table, stdout);

        if (out_file != NULL) file_write(out_file, "PAGE FAULT: Unwritable\n", 23);

        if (error_code->present) debug_print("Reason: PROTECTION VIOLATION\n");
        else debug_print("Reason: NOT PRESENT\n");
        if (error_code->write) debug_print("WRITE\n");
        if (error_code->instruction_fetch) debug_print("INSTRUCTION FETCH\n");
        if (error_code->user) debug_print("USER\n");

        debug_print("Fault VAddr: ");
        debug_print_hex((uint64_t) fault_vaddr);
        debug_print("\n");

        process_kill(current_process);
        return;
    }
}

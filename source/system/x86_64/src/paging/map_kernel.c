#include <stddef.h>

#include <paging/kernel_translation.h>
#include <paging/virtual_reservations.h>

#include <util/heap/internal.h>

#include <debug/printf.h>

#include <sys/panic.h>

#include <sys/paging/map_kernel.h>

pman_mapping_t * kernel_executable_mapping;

void sys_paging_map_kernel_regions(pman_context_t * context) {
    {
        pml4t64_entry_t * bitmap_pml4t_entry = pml4t64_map_address(
            context->top_level_table,
            paging_kernel_virtual_to_physical(paging_bitmap_pdpt),
            PAGING_BITMAP_VADDR
        );
        bitmap_pml4t_entry->present = true;
        bitmap_pml4t_entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_BITMAP_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->top_level_table,
            paging_kernel_virtual_to_physical(paging_talloc_pdpt),
            PAGING_TALLOC_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_TALLOC_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->top_level_table,
            paging_kernel_virtual_to_physical(paging_tmap_pdpt),
            PAGING_TMAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_TMAP_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->top_level_table,
            heap_pdpt_paddr,
            PAGING_HEAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_HEAP_VADDR, PDPT_SIZE);
    }

    pman_mapping_t * id_mapping = pman_context_add_map(context, PMAN_PROT_VRESERVE | PMAN_PROT_EXECUTE | PMAN_PROT_WRITE, (void *) 0, 0, 0x100000);
    if (id_mapping == NULL) panic0("Unable to map BIOS area");

    kernel_executable_mapping = pman_context_add_map(context, PMAN_PROT_EXECUTE | PMAN_PROT_WRITE, KERNEL_START, primary_region_start, KERNEL_SIZE);
}

pman_mapping_t * sys_paging_map_kernel_executable(pman_context_t * context) {
    return pman_context_add_shared(context, PMAN_PROT_EXECUTE, kernel_executable_mapping, KERNEL_START);
}

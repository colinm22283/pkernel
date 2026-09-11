#include <stddef.h>

#include <paging/kernel_translation.h>
#include <paging/virtual_reservations.h>

#include <pman/pman.h>

#include <util/heap/internal.h>

#include <debug/printf.h>

#include <sys/panic.h>

#include <sys/paging/map_kernel.h>

pman_range_t * kernel_executable_range;

void sys_paging_map_kernel_regions(pman_context_t * context) {
    {
        pml4t64_entry_t * bitmap_pml4t_entry = pml4t64_map_address(
            context->tlt,
            paging_kernel_virtual_to_physical(paging_bitmap_pdpt),
            PAGING_BITMAP_VADDR
        );
        bitmap_pml4t_entry->present = true;
        bitmap_pml4t_entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_BITMAP_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->tlt,
            paging_kernel_virtual_to_physical(paging_talloc_pdpt),
            PAGING_TALLOC_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_TALLOC_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->tlt,
            paging_kernel_virtual_to_physical(paging_tmap_pdpt),
            PAGING_TMAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_TMAP_VADDR, PDPT_SIZE);
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            context->tlt,
            heap_pdpt_paddr,
            PAGING_HEAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;

        valloc_reserve(&context->valloc, PAGING_HEAP_VADDR, PDPT_SIZE);
    }

    pman_range_t * id_range = pman_add_anon_map(
        context,
        (void *) 0,
        0x100000,
        PMAN_VRESERVE,
        PMAN_EXECUTE | PMAN_WRITE
    );
    if (id_range == NULL) panic0("Unable to map BIOS area");

    kernel_executable_range = pman_add_anon_map(
        context,
        KERNEL_START,
        KERNEL_SIZE,
        0,
        PMAN_EXECUTE | PMAN_WRITE
    );
}

pman_range_t * sys_paging_map_kernel_executable(pman_context_t * context) {
    return pman_copy_range(
        context,
        kernel_executable_range,
        KERNEL_START,
        PMAN_SHARED,
        PMAN_EXECUTE
    );
}

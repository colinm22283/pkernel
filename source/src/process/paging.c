#include <process/paging.h>

#include <paging/tables.h>
#include <paging/kernel_translation.h>
#include <paging/translation_map.h>

#include <util/heap/heap.h>

#include <util/memory/memset.h>

#include <sys/paging/page_type.h>

bool process_paging_init(process_paging_t * paging_data) {
    if (!paging_alloc_pages(&paging_data->pml4t_alloc, 1)) return false;

    paging_tmap_map(paging_data->pml4t_alloc.paddrs[0], paging_data->pml4t_alloc.vaddr);

    memset(paging_data->pml4t_alloc.vaddr, 0, sizeof(pml4t64_t));

    pml4t64_entry_t * pml4t_entry = pml4t64_map_address(paging_data->pml4t_alloc.vaddr, paging_kernel_virtual_to_physical(paging_kernel_pdpt), KERNEL_START);
    pml4t_entry->present = 1;
    pml4t_entry->read_write = 1;
    pml4t_entry->user_super = 1;

    return true;
}

void process_paging_free(process_paging_t * paging_data) {

}

bool process_paging_alloc_ex(process_paging_t * paging_data, process_paging_allocation_t * allocation, uint64_t size_bytes, bool read_write, bool execute_disable) {
    if (!paging_alloc(&allocation->kernel_allocation, size_bytes)) return false;

    allocation->user_mappings = heap_alloc(allocation->kernel_allocation.paddr_count * sizeof(paging_mapping_t));

    allocation->vaddr = allocation->kernel_allocation.vaddr;
    page_data_t * vaddr = allocation->kernel_allocation.vaddr;

    for (uint64_t i = 0; i < allocation->kernel_allocation.paddr_count; i++) {
        if (!paging_map_ex(
            (pml4t64_t *) paging_data->pml4t_alloc.vaddr,
            &allocation->user_mappings[i],
            allocation->kernel_allocation.paddrs[i],
            vaddr,
            1,
            read_write,
            execute_disable,
            true
        )) return false;

        vaddr++;
    }

    return true;
}

bool process_paging_alloc_static_ex(process_paging_t * paging_data, process_paging_allocation_t * allocation, void * user_address, uint64_t size_bytes, bool read_write, bool execute_disable) {
    if (!paging_alloc(&allocation->kernel_allocation, size_bytes)) return false;

    allocation->user_mappings = heap_alloc(allocation->kernel_allocation.paddr_count * sizeof(paging_mapping_t));

    allocation->vaddr = user_address;
    page_data_t * vaddr = user_address;

    for (uint64_t i = 0; i < allocation->kernel_allocation.paddr_count; i++) {
        if (!paging_map_ex(
            (pml4t64_t *) paging_data->pml4t_alloc.vaddr,
            &allocation->user_mappings[i],
            allocation->kernel_allocation.paddrs[i],
            vaddr,
            1,
            read_write,
            execute_disable,
            true
        )) return false;

        vaddr++;
    }

    return true;
}
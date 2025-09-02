#include <paging/mapper.h>
#include <paging/tables.h>
#include <paging/bitmap.h>
#include <paging/translation_map.h>
#include <paging/temp_page.h>

#include <util/heap/heap.h>

#include <entry_error.h>

#include <debug/vga_print.h>
#include <util/heap/internal.h>

#define MAPPER_ALLOCATION_INITIAL_SIZE (1)

static inline bool paging_map_single(
    pml4t64_t * pml4t,
    paging_table_allocation_t * pt_alloc,
    paging_table_allocation_t * pdt_alloc,
    paging_table_allocation_t * pdpt_alloc,
    uint64_t paddr,
    void * vaddr,
    bool read_write,
    bool execute_disable,
    bool user_super
) {
    uint64_t pml4t_index = ((uint64_t) vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t) vaddr >> 30) & 0x1FF;
    uint64_t pdt_index = ((uint64_t) vaddr >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t) vaddr >> 12) & 0x1FF;

    pdpt64_t * pdpt;
    if (!(*pml4t)[pml4t_index].present) {
        paging_talloc_alloc(pdpt_alloc);

        pdpt = pdpt_alloc->vaddr;

        PML4T64_SET_ADDRESS((*pml4t)[pml4t_index], pdpt_alloc->paddr);
        (*pml4t)[pml4t_index].present = true;
        (*pml4t)[pml4t_index].user_super = 1;
        (*pml4t)[pml4t_index].read_write = true;
    }
    else {
        uint64_t pdpt_paddr = PML4T64_GET_ADDRESS((*pml4t)[pml4t_index]);
        pdpt = paging_tmap_translate(pdpt_paddr);
        if (pdpt == NULL) {
            vga_print("AAAA1\n");

            kernel_entry_error(0x601);
        }
    }

    pdt64_t * pdt;
    if (!(*pdpt)[pdpt_index].present) {
        paging_talloc_alloc(pdt_alloc);

        pdt = pdt_alloc->vaddr;

        PDPT64_SET_ADDRESS((*pdpt)[pdpt_index], pdt_alloc->paddr);
        (*pdpt)[pdpt_index].present = true;
        (*pdpt)[pdpt_index].user_super = 1;
        (*pdpt)[pdpt_index].read_write = true;
    }
    else {
        uint64_t pdt_paddr = PDPT64_GET_ADDRESS((*pdpt)[pdpt_index]);
        pdt = paging_tmap_translate(pdt_paddr);
        if (pdt == NULL) vga_print("AAAA2\n");
    }

    pt64_t * pt;
    if (!(*pdt)[pdt_index].present) {
        paging_talloc_alloc(pt_alloc);

        pt = pt_alloc->vaddr;

        PDT64_SET_ADDRESS((*pdt)[pdt_index], pt_alloc->paddr);
        (*pdt)[pdt_index].present = true;
        (*pdt)[pdt_index].user_super = 1;
        (*pdt)[pdt_index].read_write = true;
    }
    else {
        uint64_t pt_paddr = PDT64_GET_ADDRESS((*pdt)[pdt_index]);
        pt = paging_tmap_translate(pt_paddr);
        if (pt == NULL) vga_print("AAAA3\n");
    }

    if (!(*pt)[pt_index].present) {
        PT64_SET_ADDRESS((*pt)[pt_index], paddr);
        (*pt)[pt_index].present = true;
        (*pt)[pt_index].read_write = read_write;
        (*pt)[pt_index].execute_disable = execute_disable;
        (*pt)[pt_index].user_super = user_super;

        invalidate_page(vaddr);

        return true;
    }
    else {
        return false;
    }
}

static inline void add_allocation(
    paging_table_allocation_t * allocation,
    paging_mapping_t * mapping,
    uint64_t * capacity
) {
    mapping->allocations[mapping->allocation_count++] = *allocation;

    if (mapping->allocation_count == *capacity) {
        *capacity *= 2;

        mapping->allocations = heap_realloc(mapping->allocations, *capacity * sizeof(paging_table_allocation_t));
    }
}

bool paging_map_ex(pml4t64_t * pml4t, paging_mapping_t * mapping, uint64_t paddr, void * vaddr, uint64_t size_pages, bool read_write, bool execute_disable, bool user_super) {
    uint64_t capacity = 1;
    mapping->allocation_count = 0;
    mapping->allocations = heap_alloc(capacity * sizeof(paging_table_allocation_t));
    mapping->vaddr = vaddr;

    mapping->size_pages = size_pages;

    paging_table_allocation_t pt_alloc, pdt_alloc, pdpt_alloc;

    uint64_t current_paddr = paddr;
    page_data_t * current_vaddr = vaddr;

    for (uint64_t i = 0; i < size_pages; i++) {
        pt_alloc.pt = NULL;
        pdt_alloc.pt = NULL;
        pdpt_alloc.pt = NULL;

        if (!paging_map_single(
            pml4t,
            &pt_alloc,
            &pdt_alloc,
            &pdpt_alloc,
            current_paddr,
            current_vaddr,
            read_write,
            execute_disable,
            user_super
        )) {
            vga_print("MAP ERROR\n");
            vga_print("index: ");
            vga_print_hex(i);
            vga_print("\n");
            vga_print("vaddr: ");
            vga_print_hex((uint64_t) current_vaddr);
            vga_print("\n");
            hlt();

            return false;
        }

        if (pt_alloc.pt != NULL) add_allocation(&pt_alloc, mapping, &capacity);
        if (pdt_alloc.pt != NULL) add_allocation(&pdt_alloc, mapping, &capacity);
        if (pdpt_alloc.pt != NULL) add_allocation(&pdpt_alloc, mapping, &capacity);

        current_paddr += 0x1000;
        current_vaddr++;
    }

    return true;
}

static inline bool paging_unmap_single(pml4t64_t * pml4t, void * vaddr) {
    uint64_t pml4t_index = ((uint64_t) vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t) vaddr >> 30) & 0x1FF;
    uint64_t pdt_index = ((uint64_t) vaddr >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t) vaddr >> 12) & 0x1FF;

    pdpt64_t * pdpt = paging_tmap_translate(PML4T64_GET_ADDRESS((*pml4t)[pml4t_index]));
    pdt64_t * pdt = paging_tmap_translate(PDPT64_GET_ADDRESS((*pdpt)[pdpt_index]));
    pt64_t * pt = paging_tmap_translate(PDT64_GET_ADDRESS((*pdt)[pdt_index]));

    if (!(*pt)[pt_index].present) return false;

    (*pt)[pt_index].present = false;

    {
        bool empty = true;
        for (uint64_t i = 0; i < 512; i++) {
            if ((*pt)[i].present) {
                empty = false;
                break;
            }
        }
        if (empty) {
            // TODO
        }
    }

    return true;
}

void paging_unmap(pml4t64_t * pml4t, paging_mapping_t * mapping) {
    for (uint64_t i = 0; i < mapping->size_pages; i++) {
        paging_unmap_single(pml4t, (char *) mapping->vaddr + (i * 0x1000));
    }
    heap_free(mapping->allocations);
}

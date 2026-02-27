#include <stdbool.h>

#include <paging/table_allocator.h>
#include <paging/tables.h>
#include <paging/kernel_translation.h>
#include <paging/bitmap.h>
#include <paging/temp_page.h>
#include <paging/translation_map.h>

#include <util/memory/memset.h>

#include <sys/paging/page_type.h>
#include <sys/paging/reload_page_table.h>

page_data_t * current_vaddr = PAGING_TALLOC_VADDR;

uint64_t talloc_pdt_paddr, talloc_pt_paddr;

uint64_t talloc_pt_count;

static inline uint64_t add_pt(void) {
    talloc_pt_paddr = bitmap_reserve();
    memset(temp_pt_map_page(talloc_pt_paddr), 0, sizeof(pt64_t));

    uint64_t pdt_index = ((uint64_t) current_vaddr >> 21) & 0x1FF;

    {
        temp_pt_map_page(talloc_pdt_paddr);
        pdt64_t * pdt = PAGING_TEMP_PT_VADDR;

        PDT64_SET_ADDRESS((*pdt)[pdt_index], talloc_pt_paddr);
        (*pdt)[pdt_index].present = true;
        (*pdt)[pdt_index].read_write = true;

        if (pdt_index == 511) {
            talloc_pdt_paddr = bitmap_reserve();
            memset(temp_pt_map_page(talloc_pdt_paddr), 0, sizeof(pdt64_t));

            pdpt64_entry_t * entry = pdpt64_map_address(
                &paging_talloc_pdpt,
                talloc_pdt_paddr,
                current_vaddr
            );
            entry->present = true;
            entry->read_write = true;
        }
    }

    {
        temp_pt_map_page(talloc_pt_paddr);
        pt64_t * pt = PAGING_TEMP_PT_VADDR;

        memset(pt, 0, sizeof(pt64_t));

        PT64_SET_ADDRESS((*pt)[0], talloc_pt_paddr);
        (*pt)[0].present = true;
        (*pt)[0].read_write = true;
    }

    reload_page_table();

    current_vaddr += 512;

    talloc_pt_count++;

    return talloc_pt_paddr;
}

void paging_talloc_init(void) {
    talloc_pdt_paddr = bitmap_reserve();
    memset(temp_pt_map_page(talloc_pdt_paddr), 0, sizeof(pdt64_t));

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            &paging_kernel_pml4t,
            paging_kernel_virtual_to_physical(paging_talloc_pdpt),
            PAGING_TALLOC_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    {
        pdpt64_entry_t * entry = pdpt64_map_address(
            &paging_talloc_pdpt,
            talloc_pdt_paddr,
            PAGING_TALLOC_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    talloc_pt_count = 0;

    add_pt();
}

bool paging_talloc_alloc(paging_table_allocation_t * alloc) {
    pt64_t * pt = (pt64_t *) PAGING_TALLOC_VADDR;

    uint64_t current_pt = 0;

    while (true) {
        for (uint16_t i = 1; i < 512; i++) {
            if (!(*pt)[i].present) {
                uint64_t paddr = bitmap_reserve();

                PT64_SET_ADDRESS((*pt)[i], paddr);
                (*pt)[i].present = true;
                (*pt)[i].read_write = true;

                reload_page_table();

                alloc->pt = pt;
                alloc->paddr = paddr;
                alloc->vaddr = ((page_data_t *) pt) + i;

                memset(alloc->vaddr, 0, sizeof(page_data_t));

                paging_tmap_map(alloc->paddr, alloc->vaddr);

                return true;
            }
        }

        current_pt++;
        if (current_pt >= talloc_pt_count) add_pt();
        pt += 512;
    }
}

void paging_talloc_free(paging_table_allocation_t * allocation) {
    paging_tmap_unmap(allocation->paddr);
}

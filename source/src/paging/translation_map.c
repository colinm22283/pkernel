#include <stddef.h>

#include <paging/translation_map.h>
#include <paging/tables.h>
#include <paging/bitmap.h>
#include <paging/kernel_translation.h>
#include <paging/temp_page.h>

#include <util/memory/memset.h>

paging_tmap_t paging_tmap;
paging_tmap_page_t * next_page_ptr = PAGING_TMAP_VADDR;

uint64_t tmap_pdt_paddr, tmap_pt_paddr;

static inline uint64_t tmap_hash_function(uint64_t paddr) {
    return (paddr >> 12) % PAGING_TMAP_BUCKETS;
}

static inline paging_tmap_page_t * add_tmap_page(void) {
    // flash(5);

    uint64_t tmap_page_paddr = bitmap_reserve();
    memset(temp_pt_map_page(tmap_page_paddr), 0, sizeof(pt64_t));

    // flash(6);

    {
        temp_pt_map_page(tmap_pt_paddr);
        pt64_t * pt = PAGING_TEMP_PT_VADDR;

        uint64_t pt_index = ((uint64_t) next_page_ptr >> 12) & 0x1FF;

        PT64_SET_ADDRESS((*pt)[pt_index], tmap_page_paddr);
        (*pt)[pt_index].present = true;
        (*pt)[pt_index].read_write = true;

        next_page_ptr++;

        if (pt_index == 511) {
            tmap_pt_paddr = bitmap_reserve();
            memset(temp_pt_map_page(tmap_pt_paddr), 0, sizeof(pt64_t));

            temp_pt_map_page(tmap_pdt_paddr);
            pdt64_t * pdt = PAGING_TEMP_PT_VADDR;

            uint64_t pdt_index = ((uint64_t) next_page_ptr >> 21) & 0x1FF;

            PDT64_SET_ADDRESS((*pdt)[pdt_index], tmap_pt_paddr);
            (*pdt)[pdt_index].present = true;
            (*pdt)[pdt_index].read_write = true;

            if (pdt_index == 511) {
                tmap_pdt_paddr = bitmap_reserve();
                memset(temp_pt_map_page(tmap_pdt_paddr), 0, sizeof(pdt64_t));

                pdpt64_entry_t * entry = pdpt64_map_address(&paging_tmap_pdpt, tmap_pdt_paddr, next_page_ptr);
                entry->present = true;
                entry->read_write = true;
            }
        }
    }

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    (next_page_ptr - 1)->paddr = tmap_page_paddr;
    (next_page_ptr - 1)->entry_count = 0;

    return next_page_ptr - 1;
}

void paging_tmap_init(void) {
    // flash(2);
    // flash(2);
    // flash(2);
    // flash(2);
    // flash(1);
    // flash(2);

    tmap_pdt_paddr = bitmap_reserve();
    tmap_pt_paddr = bitmap_reserve();

    memset(temp_pt_map_page(tmap_pt_paddr), 0, sizeof(pt64_t));

    // flash(3);

    {
        temp_pt_map_page(tmap_pdt_paddr);
        pdt64_t * pdt = PAGING_TEMP_PT_VADDR;

        memset(pdt, 0, sizeof(pdt64_t));

        pdt64_entry_t * entry = pdt64_map_address(pdt, tmap_pt_paddr, PAGING_TMAP_VADDR);
        entry->present = true;
        entry->read_write = true;
    }

    {
        pdpt64_entry_t * entry = pdpt64_map_address(&paging_tmap_pdpt, tmap_pdt_paddr, PAGING_TMAP_VADDR);
        entry->present = true;
        entry->read_write = true;
    }

    {
        pml4t64_entry_t * entry = pml4t64_map_address(
            &paging_kernel_pml4t,
            paging_kernel_virtual_to_physical(paging_tmap_pdpt),
            PAGING_TMAP_VADDR
        );
        entry->present = true;
        entry->read_write = true;
    }

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    // flash(4);

    for (uint32_t i = 0; i < PAGING_TMAP_BUCKETS; i++) {
        paging_tmap.root_entries[i] = add_tmap_page();
        paging_tmap.root_entries[i]->next = NULL;
    }
}

void paging_tmap_map(uint64_t paddr, void * vaddr) {
    uint64_t hash_idx = tmap_hash_function(paddr);

    paging_tmap_page_t * tmap_page = paging_tmap.root_entries[hash_idx];

    while (tmap_page != NULL) {
        if (tmap_page->entry_count != PAGING_TMAP_PAGE_ENTRIES) {
            tmap_page->entries[tmap_page->entry_count].paddr = paddr;
            tmap_page->entries[tmap_page->entry_count].vaddr = vaddr;
            tmap_page->entry_count++;

            return;
        }

        tmap_page = tmap_page->next;
    }

    paging_tmap_page_t * old_page = paging_tmap.root_entries[hash_idx];

    paging_tmap.root_entries[hash_idx] = add_tmap_page();
    paging_tmap.root_entries[hash_idx]->next = old_page;

    paging_tmap.root_entries[hash_idx]->entries[paging_tmap.root_entries[hash_idx]->entry_count].paddr = paddr;
    paging_tmap.root_entries[hash_idx]->entries[paging_tmap.root_entries[hash_idx]->entry_count].vaddr = vaddr;
    paging_tmap.root_entries[hash_idx]->entry_count++;
}

bool paging_tmap_unmap(uint64_t paddr) {
    uint64_t hash_idx = tmap_hash_function(paddr);

    paging_tmap_page_t * tmap_page = paging_tmap.root_entries[hash_idx];

    while (tmap_page != NULL) {
        for (uint32_t i = 0; i < tmap_page->entry_count; i++) {
            if (tmap_page->entries[i].paddr == paddr) {
                for (uint32_t j = i + 1; j < tmap_page->entry_count; j++) {
                    tmap_page->entries[j - 1] = tmap_page->entries[j];
                }

                tmap_page->entry_count--;

                return true;
            }
        }

        tmap_page = tmap_page->next;
    }

    return false;
}

void * paging_tmap_translate(uint64_t paddr) {
    uint64_t hash_idx = tmap_hash_function(paddr);

    paging_tmap_page_t * tmap_page = paging_tmap.root_entries[hash_idx];

    while (tmap_page != NULL) {
        for (uint32_t i = 0; i < tmap_page->entry_count; i++) {
            if (tmap_page->entries[i].paddr == paddr) return tmap_page->entries[i].vaddr;
        }

        tmap_page = tmap_page->next;
    }

    return NULL;
}

uint64_t paging_tmap_translate_tables(pml4t64_t * pml4t, void * vaddr) {
    uint64_t pml4t_index = ((uint64_t) vaddr >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t) vaddr >> 30) & 0x1FF;
    uint64_t pdt_index = ((uint64_t) vaddr >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t) vaddr >> 12) & 0x1FF;

    uint64_t _pdtmap_pt_paddr = PML4T64_GET_ADDRESS((*pml4t)[pml4t_index]);
    pdpt64_t * pdpt = paging_tmap_translate(_pdtmap_pt_paddr);

    uint64_t _tmap_pdt_paddr = PDPT64_GET_ADDRESS((*pdpt)[pdpt_index]);
    pdpt64_t * pdt = paging_tmap_translate(_tmap_pdt_paddr);

    uint64_t _tmap_pt_paddr = PDT64_GET_ADDRESS((*pdt)[pdt_index]);
    pdpt64_t * pt = paging_tmap_translate(_tmap_pt_paddr);

    return PT64_GET_ADDRESS((*pt)[pt_index]);
}
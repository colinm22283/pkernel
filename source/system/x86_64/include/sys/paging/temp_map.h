#pragma once

#include <paging/kernel_translation.h>
#include <paging/tables.h>
#include <paging/virtual_reservations.h>

#include <sys/paging/pml4t.h>
#include <sys/paging/pdpt.h>
#include <sys/paging/pdt.h>
#include <sys/paging/pt.h>
#include <sys/paging/page_type.h>

#include <sys/paging/load_page_table.h>
#include <sys/paging/invalidate_page.h>

static inline void * temp_pt_map_page(uint64_t paddr) {
    pt64_entry_t * pt_entry = pt64_map_address(&paging_temp_pt, paddr, PAGING_TEMP_PT_VADDR);
    pt_entry->present = true;
    pt_entry->read_write = true;

    invalidate_page(PAGING_TEMP_PT_VADDR);

    return PAGING_TEMP_PT_VADDR;
}

static inline void * temp_pt_map_page_slot(uint64_t paddr, uint64_t slot) {
    page_data_t * vaddr = ((page_data_t *) PAGING_TEMP_PT_VADDR) + slot;

    pt64_entry_t * pt_entry = pt64_map_address(&paging_temp_pt, paddr, vaddr);
    pt_entry->present = true;
    pt_entry->read_write = true;

    invalidate_page(vaddr);

    return vaddr;
}

static inline void * temp_pt_map_2mb(uint64_t paddr) {
    pt64_map_2mb(&paging_temp_pt, paddr);

    for (page_data_t * page = PAGING_TEMP_PT_VADDR; page != ((page_data_t *) PAGING_TEMP_PT_VADDR) + 512; page++) {
        invalidate_page(page);
    }

    return PAGING_TEMP_PT_VADDR;
}


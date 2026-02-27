#include <stdint.h>

#include <paging/region.h>
#include <paging/tables.h>
#include <paging/kernel_translation.h>
#include <paging/temp_page.h>
#include <paging/virtual_reservations.h>

#include <memory/memory_map.h>

#include <util/memory/memset.h>
#include <util/math/div_up.h>
#include <util/math/min.h>

#include <sys/paging/page_size.h>
#include <paging/bitmap.h>

#include <entry_error.h>

uint64_t bitmap_available_pages;
uint64_t bitmap_size;
uint64_t bitmap_paddr;

void paging_bitmap_init(void) {
    pml4t64_entry_t * bitmap_pml4t_entry = pml4t64_map_address(
        &paging_kernel_pml4t,
        paging_kernel_virtual_to_physical(paging_bitmap_pdpt),
        PAGING_BITMAP_VADDR
    );
    bitmap_pml4t_entry->present = true;
    bitmap_pml4t_entry->read_write = true;

    memset(paging_temp_pt, 0, sizeof(pt64_t));

    uint64_t paging_region_size = paging_region_end - paging_region_start;

    uint64_t bitmap_overestimate = DIV_UP(paging_region_size / (PAGE_SIZE * 8), 8) * 8;

    uint64_t pt_region_overestimate = DIV_UP(bitmap_overestimate, PAGE_SIZE * 512) * PAGE_SIZE;
    uint64_t pdt_region_overestimate = DIV_UP(bitmap_overestimate, PAGE_SIZE * 512 * 512) * PAGE_SIZE;

    uint64_t remaining_space = paging_region_size - pt_region_overestimate - pdt_region_overestimate;

    bitmap_size = bitmap_overestimate;
    bitmap_available_pages = remaining_space / PAGE_SIZE;

    const uint64_t pt_count = DIV_UP(bitmap_size, PAGE_SIZE * 512);
    const uint64_t pdt_count = DIV_UP(bitmap_size, PAGE_SIZE * 512 * 512);

    bitmap_paddr = paging_region_start + PAGE_SIZE * (pt_count + pdt_count);

    allocation_region_start = DIV_UP(bitmap_paddr + bitmap_size, PAGE_SIZE) * PAGE_SIZE;
    allocation_region_end = paging_region_end;

    {
        uint64_t pt_paddr = paging_region_start;

        for (uint64_t i = 0; i < pt_count; i++) {
            temp_pt_map_page(pt_paddr);

            pt64_t * pt = PAGING_TEMP_PT_VADDR;
            memset(pt, 0, sizeof(pt64_t));

            pt64_map_2mb(pt, bitmap_paddr);

            bitmap_paddr += PAGE_SIZE * 512;

            pt_paddr += sizeof(pt64_t);
        }
    }

    {
        uint64_t pt_paddr = paging_region_start;
        uint64_t pdt_paddr = paging_region_start + PAGE_SIZE * pt_count;

        char * vaddr = (char *) PAGING_BITMAP_VADDR;

        uint64_t pt_remaining = pt_count;
        for (uint64_t i = 0; i < pdt_count; i++) {
            temp_pt_map_page(pdt_paddr);

            pdt64_t * pdt = PAGING_TEMP_PT_VADDR;
            memset(pdt, 0, sizeof(pdt64_t));

            for (uint64_t j = 0; j < MIN(512, pt_remaining); j++) {
                pdt64_entry_t * pdt_entry = pdt64_map_address(pdt, pt_paddr, vaddr);
                pdt_entry->present = true;
                pdt_entry->read_write = true;

                vaddr += PAGE_SIZE * 512;
                pt_paddr += sizeof(pt64_t);
                pt_remaining--;
            }

            pdt_paddr += sizeof(pdt64_t);
        }
    }

    {
        uint64_t pdt_paddr = paging_region_start + PAGE_SIZE * pt_count;

        char * vaddr = (char *) PAGING_BITMAP_VADDR;

        for (uint64_t i = 0; i < pdt_count; i++) {
            pdpt64_entry_t * pdpt_entry = pdpt64_map_address(&paging_bitmap_pdpt, pdt_paddr, vaddr);
            pdpt_entry->present = true;
            pdpt_entry->read_write = true;

            vaddr += PAGE_SIZE * 512 * 512;
            pdt_paddr += sizeof(pdt64_t);
        }
    }

    memset(PAGING_BITMAP_VADDR, 0, bitmap_size);
}

uint64_t bitmap_reserve(void) {
    for (uint64_t i = 0; i < bitmap_available_pages; i++) {
        if (!paging_bitmap_get_index(i)) {
            paging_bitmap_set_index(i);

            return allocation_region_start + i * PAGE_SIZE;
        }
    }

    return 0;
}

uint64_t bitmap_reserve_contiguous(uint64_t size_pages) {
    for (uint64_t i = 0; i < bitmap_available_pages; i++) {
        bool valid = true;

        for (uint64_t j = 0; j < size_pages; j++) {
            if (paging_bitmap_get_index(i + j)) {
                valid = false;

                i += j;

                break;
            }
        }

        if (valid) {
            for (uint64_t j = 0; j < size_pages; j++) paging_bitmap_set_index(i + j);

            return allocation_region_start + i * PAGE_SIZE;
        }
    }

    return 0;
}

error_number_t bitmap_free(uint64_t address, uint64_t size_pages) {
    error_number_t result = ERROR_OK;

    for (uint64_t i = 0; i < size_pages; i++) {
        if (!paging_bitmap_get_index(address / PAGE_SIZE + i)) result = ERROR_BITMAP_NOT_ALLOCATED;

        paging_bitmap_clear_index(address / PAGE_SIZE + i);
    }

    return result;
}
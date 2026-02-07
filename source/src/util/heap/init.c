#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <paging/virtual_reservations.h>
#include <paging/tables.h>
#include <paging/kernel_translation.h>
#include <paging/bitmap.h>
#include <paging/temp_page.h>

#include <sysfs/sysfs.h>

#include <util/math/div_up.h>
#include <util/memory/memset.h>
#include <util/string/writestr.h>

#include <sys/paging/page_type.h>
#include <sys/paging/page_size.h>

enum {
    SYSFS_CAPACITY = 0,
    SYSFS_USAGE = 1,
};

int64_t heap_sysfs_read(uint64_t id, char * data, uint64_t size, uint64_t offset) {
    if (offset != 0) return 0;

    switch (id) {
        case SYSFS_CAPACITY: {
            return (int64_t) writestr(data, size, offset, heap_total());
        } break;

        case SYSFS_USAGE: {
            return (int64_t) writestr(data, size, offset, heap_usage());
        } break;

        default: break;
    }
    return 0;
}

int64_t heap_sysfs_write(uint64_t id, const char * data, uint64_t size, uint64_t offset) {
    switch (id) {
        default: break;
    }

    return 0;
}

void heap_init(void) {
    {
        uint64_t mappings_remaining = DIV_UP(HEAP_INITIAL_SIZE, PAGE_SIZE);

        page_data_t * vaddr = PAGING_HEAP_VADDR;

        while (true) {
            uint64_t pdpt_paddr = bitmap_reserve();
            pml4t64_entry_t * pml4t_entry = pml4t64_map_address(&paging_kernel_pml4t, pdpt_paddr, vaddr);
            pml4t_entry->present = true;
            pml4t_entry->read_write = true;

            pdpt64_t * pdpt = temp_pt_map_page_slot(pdpt_paddr, 0);
            memset(pdpt, 0, sizeof(pdpt64_t));

            for (uint64_t i = 0; i < 512; i++) {
                uint64_t pdt_paddr = bitmap_reserve();
                pdpt64_entry_t * pdpt_entry = pdpt64_map_address(pdpt, pdt_paddr, vaddr);
                pdpt_entry->present = true;
                pdpt_entry->read_write = true;

                pdt64_t * pdt = temp_pt_map_page_slot(pdt_paddr, 1);
                memset(pdt, 0, sizeof(pdt64_t));

                for (uint64_t j = 0; j < 512; j++) {
                    uint64_t pt_paddr = bitmap_reserve();
                    pdt64_entry_t * pdt_entry = pdt64_map_address(pdt, pt_paddr, vaddr);
                    pdt_entry->present = true;
                    pdt_entry->read_write = true;

                    pt64_t * pt = temp_pt_map_page_slot(pt_paddr, 2);
                    memset(pt, 0, sizeof(pt64_t));

                    for (uint64_t k = 0; k < 512; k++) {
                        uint64_t data_paddr = bitmap_reserve();
                        pt64_entry_t * pt_entry = pt64_map_address(pt, data_paddr, vaddr);
                        pt_entry->present = true;
                        pt_entry->read_write = true;

                        vaddr++;
                        mappings_remaining--;

                        if (mappings_remaining == 0) break;
                    }

                    if (mappings_remaining == 0) break;
                }

                if (mappings_remaining == 0) break;
            }

            if (mappings_remaining == 0) break;
        }
    }

    alloc_size = 0;

    uint64_t sector_size = HEAP_INITIAL_SIZE - 2 * sizeof(heap_tag_t);

    head_tag = PAGING_HEAP_VADDR;
    tail_tag = (heap_tag_t *) ((intptr_t) PAGING_HEAP_VADDR + sector_size + sizeof(heap_tag_t));

    head_tag->next_size = sector_size;
    head_tag->prev_size = 0;
    head_tag->next_reserved = false;

    tail_tag->prev_size = sector_size;
    tail_tag->next_size = 0;
    tail_tag->next_reserved = false;
}

void heap_init_sysfs(void) {
    sysfs_add_entry("heap/usage", SYSFS_USAGE, heap_sysfs_read, heap_sysfs_write);
    sysfs_add_entry("heap/capacity", SYSFS_CAPACITY, heap_sysfs_read, heap_sysfs_write);
}
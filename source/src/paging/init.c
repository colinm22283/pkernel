#include <paging/init.h>
#include <paging/tables.h>
#include <paging/region.h>
#include <paging/kernel_translation.h>
#include <paging/bitmap.h>
#include <paging/translation_map.h>
#include <paging/table_allocator.h>
#include <paging/virtual_allocator.h>
#include <paging/mapper.h>
#include <paging/manager.h>

#include <memory/kernel.h>
#include <memory/data.h>
#include <memory/stack.h>
#include <memory/text.h>
#include <memory/modules.h>

#include <util/memory/memset.h>
#include <util/math/div_up.h>

#include <sys/msr/set_msr.h>
#include <sys/paging/load_page_table.h>
#include <sys/paging/page_type.h>
#include <sys/paging/page_size.h>

void paging_init() {
    paging_region_start = primary_region_start + KERNEL_SIZE;
    paging_region_end = primary_region_end;

    paging_region_start = DIV_UP(paging_region_start, PAGE_SIZE) * PAGE_SIZE;
    paging_region_end = (paging_region_end / PAGE_SIZE) * PAGE_SIZE;

    // set the NXE flag
    set_msr(MSR_IA32_EFER, 1 << 11);

    memset(paging_kernel_pml4t,    0, sizeof(pml4t64_t));
    memset(paging_kernel_pdpt,     0, sizeof(pdpt64_t));
    memset(paging_kernel_pdt,      0, sizeof(pdt64_t));
    memset(paging_kernel_pt,  0, sizeof(pt64_t));
    memset(paging_identity_pdt,    0, sizeof(pdt64_t));
    memset(paging_identity_pt,     0, sizeof(pt64_t));
    memset(paging_bitmap_pdpt,     0, sizeof(pdpt64_t));
    memset(paging_tmap_pdpt,       0, sizeof(pdpt64_t));
    memset(paging_talloc_pdpt,     0, sizeof(pdpt64_t));
    memset(paging_valloc_pdpt,     0, sizeof(pdpt64_t));
    memset(paging_temp_pt,         0, sizeof(pt64_t));

    pml4t64_entry_t * pml4t_entry = pml4t64_map_address(
        &paging_kernel_pml4t,
        paging_kernel_virtual_to_physical(paging_kernel_pdpt),
        KERNEL_START
    );
    pml4t_entry->present = 1;
    pml4t_entry->read_write = 1;
    pml4t_entry->user_super = 1;

    pdpt64_entry_t * pdpt_entry  = pdpt64_map_address(
        &paging_kernel_pdpt,
        paging_kernel_virtual_to_physical(paging_kernel_pdt),
        KERNEL_START
    );
    pdpt_entry->present = 1;
    pdpt_entry->read_write = 1;
    pdpt_entry->user_super = 1;

    pdt64_entry_t * pdt_entry = pdt64_map_address(
        &paging_kernel_pdt,
        paging_kernel_virtual_to_physical(paging_kernel_pt),
        KERNEL_START
    );
    pdt_entry->present = 1;
    pdt_entry->read_write = 1;
    pdt_entry->user_super = 1;

    {
        uint64_t address = primary_region_start;

        uint64_t data_page_index = DIV_UP(TEXT_SIZE, PAGE_SIZE);
        uint64_t end_page_index = DIV_UP(TEXT_SIZE, PAGE_SIZE) + DIV_UP(DATA_SIZE, PAGE_SIZE);

        for (uint64_t i = 0; i < data_page_index; i++) {
            PT64_SET_ADDRESS(paging_kernel_pt[i], address);
            paging_kernel_pt[i].present = true;
            paging_kernel_pt[i].read_write = false;
            paging_kernel_pt[i].user_super = true;

            address += PAGE_SIZE;
        }

        for (uint64_t i = data_page_index; i < end_page_index; i++) {
            PT64_SET_ADDRESS(paging_kernel_pt[i], address);
            paging_kernel_pt[i].present = true;
            paging_kernel_pt[i].read_write = false;

            address += PAGE_SIZE;
        }
    }

    PDPT64_SET_ADDRESS(paging_kernel_pdpt[0], paging_kernel_virtual_to_physical(paging_identity_pdt));
    paging_kernel_pdpt[0].present = 1;
    paging_kernel_pdpt[0].read_write = 1;
    PDT64_SET_ADDRESS(paging_identity_pdt[0], paging_kernel_virtual_to_physical(paging_identity_pt));
    paging_identity_pdt[0].present = 1;
    paging_identity_pdt[0].read_write = 1;
    pt64_map_2mb(&paging_identity_pt, 0);
    // {
    //     uint64_t paddr = 0xA0000;
    //     page_data_t * vaddr = (page_data_t *) 0xA0000;
    //
    //     for (uint8_t i = 0; i < 38; i++) {
    //         pt64_entry_t * identity_pt_entry = pt64_map_address(&paging_identity_pt, paddr, vaddr);
    //         identity_pt_entry->present = true;
    //         identity_pt_entry->read_write = true;
    //         identity_pt_entry->execute_disable = true;
    //
    //         paddr += 0x1000;
    //         vaddr++;
    //     }
    // }

    pdt64_entry_t * temp_pdt_entry = pdt64_map_address(
        &paging_kernel_pdt,
        paging_kernel_virtual_to_physical(paging_temp_pt),
        PAGING_TEMP_PT_VADDR
    );
    temp_pdt_entry->present = true;
    temp_pdt_entry->read_write = true;

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    paging_bitmap_init();

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    paging_tmap_init();

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    paging_talloc_init();

    load_page_table((void *) paging_kernel_virtual_to_physical(paging_kernel_pml4t));

    paging_tmap_map(paging_kernel_virtual_to_physical(paging_kernel_pml4t), paging_kernel_pml4t);
    paging_tmap_map(paging_kernel_virtual_to_physical(paging_kernel_pdpt), paging_kernel_pdpt);
    paging_tmap_map(paging_kernel_virtual_to_physical(paging_kernel_pdt), paging_kernel_pdt);
    paging_tmap_map(paging_kernel_virtual_to_physical(paging_kernel_pt), paging_kernel_pt);

    paging_tmap_map(paging_kernel_virtual_to_physical(paging_identity_pdt), paging_identity_pdt);
    paging_tmap_map(paging_kernel_virtual_to_physical(paging_identity_pt), paging_identity_pt);
}

bool paging_init_stage2(void) {
    // paging_mapping_t mapping;
    //
    // if (!paging_map_ex(
    //     pman_kernel_context()->top_level_table,
    //     &mapping,
    //     primary_region_start + ((uint64_t) MODULES_TEXT - (uint64_t) KERNEL_START),
    //     MODULES_TEXT,
    //     DIV_UP(MODULES_TEXT_SIZE, PAGE_SIZE),
    //     false,
    //     false,
    //     true
    // )) return false;
    //
    // if (!paging_map_ex(
    //     pman_kernel_context()->top_level_table,
    //     &mapping,
    //     primary_region_start + ((uint64_t) MODULES_DATA - (uint64_t) KERNEL_START),
    //     MODULES_DATA,
    //     DIV_UP(MODULES_DATA_SIZE, PAGE_SIZE),
    //     false,
    //     false,
    //     true
    // )) return false;

    return true;
}
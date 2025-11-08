#include <paging/allocator.h>
#include <paging/bitmap.h>

#include <util/heap/heap.h>

#include <sys/paging/page_type.h>

bool paging_alloc_pages(paging_allocation_t * allocation, uint64_t size_pages) {
    allocation->vaddr = paging_valloc_alloc(size_pages);

    allocation->size_pages = size_pages;
    allocation->bitmap_level = 0;
    uint64_t bytes_per_level = 0x1000;

    uint64_t pages_per_level = bytes_per_level / 0x1000;
    uint64_t bitmap_allocations = DIV_UP(size_pages, pages_per_level);

    allocation->paddr_count = bitmap_allocations;
    allocation->paddrs = heap_alloc(allocation->paddr_count * sizeof(uint64_t));

    allocation->mapping_count = bitmap_allocations;
    allocation->mappings = heap_alloc(allocation->mapping_count * sizeof(paging_mapping_t));

    for (uint64_t i = 0; i < allocation->paddr_count; i++) allocation->paddrs[i] = bitmap_reserve_level(allocation->bitmap_level);

    page_data_t * vaddr = allocation->vaddr;

    for (uint64_t i = 0; i < allocation->paddr_count; i++) {
        if (!paging_map(
            &allocation->mappings[i],
            allocation->paddrs[i],
            vaddr,
            pages_per_level
        )) return false;

        vaddr += pages_per_level;
    }

    return true;
}

bool paging_free(paging_allocation_t * allocation) {
    for (uint64_t i = 0; i < allocation->mapping_count; i++) {
        paging_unmap(&allocation->mappings[i]);
    }

    for (uint64_t i = 0; i < allocation->paddr_count; i++) {
        if (!bitmap_free_level(allocation->bitmap_level, allocation->paddrs[i])) return false;
    }

    paging_valloc_free(allocation->vaddr, allocation->size_pages);

    heap_free(allocation->paddrs);
    heap_free(allocation->mappings);

    return true;
}
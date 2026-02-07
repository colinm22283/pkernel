#include <paging/physical_allocator.h>
#include <paging/bitmap.h>

#include <util/heap/heap.h>
#include <util/math/div_up.h>
#include <sys/paging/page_size.h>

error_number_t palloc_alloc(palloc_t * palloc, uint64_t size) {
    uint64_t size_pages = DIV_UP(size, PAGE_SIZE);

    palloc->type = PALLOC_ALLOC;
    palloc->size_pages = size_pages;
    palloc->paddr_count = size_pages;
    palloc->paddrs = heap_alloc_debug(size_pages * sizeof(uint64_t), "palloc_t::paddrs normal");

    for (uint64_t i = 0; i < size_pages; i++) {
        palloc->paddrs[i] = bitmap_reserve();
    }

    return ERROR_OK;
}

error_number_t palloc_alloc_contiguous(palloc_t * palloc, uint64_t size, uint64_t paddr) {
    if (paddr == 0) {
        uint64_t size_pages = DIV_UP(size, PAGE_SIZE);

        palloc->type = PALLOC_CONTIGUOUS_ALLOC;
        palloc->size_pages = size_pages;
        palloc->paddr_count = 1;
        palloc->paddrs = heap_alloc_debug(sizeof(uint64_t), "palloc_t::paddrs contiguous alloc");

        palloc->paddrs[0] = bitmap_reserve_contiguous(size_pages);
    }
    else {
        uint64_t size_pages = DIV_UP(size, PAGE_SIZE);

        palloc->type = PALLOC_CONTIGUOUS_MAP;
        palloc->size_pages = size_pages;
        palloc->paddr_count = 1;
        palloc->paddrs = heap_alloc_debug(sizeof(uint64_t), "palloc_t::paddrs contiguous map");

        palloc->paddrs[0] = paddr;
    }

    return ERROR_OK;
}

error_number_t palloc_free(palloc_t * palloc) {
    switch (palloc->type) {
        case PALLOC_ALLOC: {
            for (uint64_t i = 0; i < palloc->paddr_count; i++) {
                bitmap_free(palloc->paddrs[i], 1);
            }

            heap_free(palloc->paddrs);
        } break;

        case PALLOC_CONTIGUOUS_ALLOC: {
            bitmap_free(palloc->paddrs[0], palloc->size_pages);

            heap_free(palloc->paddrs);
        } break;

        case PALLOC_CONTIGUOUS_MAP: {
            heap_free(palloc->paddrs);
        } break;
    }

    return ERROR_OK;
}
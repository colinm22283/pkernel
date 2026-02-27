#pragma once

#include <paging/translation_map.h>

#define PAGING_TEMP_PT_VADDR                  ((void *) 0xD0000000)

#define PAGING_BITMAP_VADDR           ((uint64_t *) 0x008000000000)
#define PAGING_TMAP_VADDR   ((paging_tmap_page_t *) 0x018000000000)
#define PAGING_TALLOC_VADDR               ((void *) 0x028000000000)
#define PAGING_HEAP_VADDR                 ((void *) 0x038000000000)

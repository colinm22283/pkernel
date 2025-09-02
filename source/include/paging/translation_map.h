#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <sys/paging/pml4t.h>

#define PAGING_TMAP_BUCKETS (256)
#define PAGING_TMAP_PAGE_ENTRIES ((0x1000 - sizeof(uint64_t) - sizeof(union paging_tmap_page_s *) - sizeof(uint32_t)) / sizeof(paging_tmap_entry_t))

typedef struct {
    uint64_t paddr;
    void * vaddr;
} paging_tmap_entry_t;

typedef union paging_tmap_page_s {
    struct {
        uint64_t paddr;

        union paging_tmap_page_s * next;

        uint32_t entry_count;
        paging_tmap_entry_t entries[PAGING_TMAP_PAGE_ENTRIES];
    };
    char _page_padding[0x1000];
} paging_tmap_page_t;

typedef struct {
    paging_tmap_page_t * root_entries[PAGING_TMAP_BUCKETS];
} paging_tmap_t;

extern paging_tmap_t paging_tmap;

void paging_tmap_init(void);

void paging_tmap_map(uint64_t paddr, void * vaddr);
bool paging_tmap_unmap(uint64_t paddr);
void * paging_tmap_translate(uint64_t paddr);

uint64_t paging_tmap_translate_tables(pml4t64_t * pml4t, void * vaddr);
#pragma once

#include <stdint.h>

#include <sys/paging/pml4t.h>
#include <sys/paging/pdpt.h>
#include <sys/paging/pdt.h>
#include <sys/paging/pt.h>
#include <sys/paging/page_size.h>

static inline pml4t64_entry_t * pml4t_index(pml4t64_t * table, void * vaddr) {
    return &((*table)[((intptr_t) vaddr) / (PAGE_SIZE * PAGE_SIZE * PAGE_SIZE * PAGE_SIZE)]);
}

static inline pdpt64_entry_t * pdpt_index(pdpt64_t * table, void * vaddr) {
    return &((*table)[((intptr_t) vaddr) / (PAGE_SIZE * PAGE_SIZE * PAGE_SIZE)]);
}

static inline pdt64_entry_t * pdt_index(pdt64_t * table, void * vaddr) {
    return &((*table)[((intptr_t) vaddr) / (PAGE_SIZE * PAGE_SIZE)]);
}

static inline pt64_entry_t * pt_index(pt64_t * table, void * vaddr) {
    return &((*table)[((intptr_t) vaddr) / PAGE_SIZE]);
}

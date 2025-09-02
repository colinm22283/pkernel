#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <defs.h>

#include <sys/paging/pdpt.h>

typedef struct {
    uint8_t  present         :  1;
    uint8_t  read_write      :  1;
    uint8_t  user_super      :  1;
    uint8_t  write_through   :  1;
    uint8_t  cache_disable   :  1;
    uint8_t  accessed        :  1;
    uint8_t  _available2     :  1;
    uint8_t  _reserved       :  1;
    uint8_t  _available1     :  4;
    uint64_t address         : 40;
    uint16_t _available0     : 11;
    uint8_t  execute_disable :  1;
} __PACKED pml4t64_entry_t;

typedef pml4t64_entry_t __ALIGNED(4096) pml4t64_t[512];

#define PML4T64_SET_ADDRESS(pml4t_entry, _address) do { (pml4t_entry).address = ((uint64_t) (intptr_t) _address) >> 12; } while (0)
#define PML4T64_GET_ADDRESS(pml4t_entry) ((uint64_t) (((uint64_t) (intptr_t) (pml4t_entry).address) << 12))
#define NULL_PML4T64_ENTRY ((pml4t64_entry_t) { .present = 0, .read_write = 0, .user_super = 0, .cache_disable = 0, .accessed = 0, ._available2 = 0, ._reserved = 0, ._available1 = 0, .address = 0, ._available0 = 0, .execute_disable = 0, })

static inline pml4t64_entry_t * pml4t64_map_address(pml4t64_t * pml4t, uint64_t pdpt, void * virtual_address) {
    PML4T64_SET_ADDRESS((*pml4t)[((uint64_t) (intptr_t) virtual_address >> 39) & 0x1FF], pdpt);
    return &(*pml4t)[((uint64_t) (intptr_t) virtual_address >> 39) & 0x1FF];
}

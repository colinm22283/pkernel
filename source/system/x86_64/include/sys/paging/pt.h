#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <defs.h>
#include <sys/paging/page_size.h>

typedef struct {
    uint8_t  present         :  1;
    uint8_t  read_write      :  1;
    uint8_t  user_super      :  1;
    uint8_t  write_through   :  1;
    uint8_t  cache_disable   :  1;
    uint8_t  accessed        :  1;
    uint8_t  _available2     :  1;
    uint8_t  page_size       :  1;
    uint8_t  _available1     :  4;
    uint64_t address         : 40;
    uint16_t _available0     :  7;
    uint8_t  protection_key  :  4;
    uint8_t  execute_disable :  1;
} __PACKED pt64_entry_t;

typedef pt64_entry_t __ALIGNED(4096) pt64_t[512];

#define PT64_SET_ADDRESS(pt_entry, _address) do { pt_entry.address = ((uint64_t) _address) >> 12; } while (0)
#define PT64_GET_ADDRESS(pt_entry) ((uint64_t)(((uint64_t) (pt_entry).address) << 12))
#define NULL_PT64_ENTRY ((pt64_entry_t) { .present = 0, .read_write = 0, .user_super = 0, .cache_disable = 0, .accessed = 0, ._available2 = 0, ._available1 = 0, .address = 0, ._available0 = 0, .protection_key = 0, .execute_disable = 0, })

static inline pt64_entry_t * pt64_map_address(pt64_t * pt, uint64_t physical_address, void * virtual_address) {
    PT64_SET_ADDRESS((*pt)[((intptr_t) virtual_address >> 12) & 0x1FF], physical_address);
    return &(*pt)[((intptr_t) virtual_address >> 12) & 0x1FF];
}

static inline void pt64_map_2mb(pt64_t * pt64, uint64_t address) {
    for (uint16_t i = 0; i < 512; i++) {
        PT64_SET_ADDRESS((*pt64)[i], address);
        (*pt64)[i].present = true;
        (*pt64)[i].read_write = true;

        address += PAGE_SIZE;
    }
}

static inline void pt64_map_2mb_ex(pt64_t * pt64, uint64_t address, bool present, bool read_write, bool user_super) {
    for (uint16_t i = 0; i < 512; i++) {
        PT64_SET_ADDRESS((*pt64)[i], address);
        (*pt64)[i].present = present;
        (*pt64)[i].read_write = read_write;
        (*pt64)[i].user_super = user_super;

        address += PAGE_SIZE;
    }
}
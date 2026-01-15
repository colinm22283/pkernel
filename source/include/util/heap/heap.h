#pragma once

#include <stdint.h>

#include <util/heap/internal.h>

#include <sys/debug/print.h>

void heap_init(void);
void heap_init_sysfs(void);

void * heap_alloc(uint64_t size_bytes);
void heap_free(void * alloc);
void * heap_realloc(void * alloc, uint64_t size_bytes);

static inline void * heap_alloc_debug(uint64_t size_bytes, const char * name) {
#ifdef HEAP_DEBUG
    debug_print("BEGIN ALLOC: ");
    debug_print(name);
    debug_print(", 0x");
    debug_print_hex(size_bytes);
    debug_print("\n");
#endif

    void * ret = heap_alloc(size_bytes);

#ifdef HEAP_DEBUG
    debug_print("ALLOC: ");
    debug_print(name);
    debug_print(", 0x");
    debug_print_hex((intptr_t) ret);
    debug_print("\n");

    ((heap_tag_t *) ret - 1)->name = name;
#endif

    return ret;
}

uint64_t heap_total(void);
uint64_t heap_usage(void);

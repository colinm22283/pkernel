#pragma once

#include <stdint.h>
#include <stdbool.h>

// #define HEAP_DEBUG

#define HEAP_INITIAL_SIZE (0x10000)

typedef struct {
    bool next_reserved;
    uint64_t prev_size;

#ifdef HEAP_DEBUG
    const char * name;
#endif

    uint64_t next_size;
} heap_tag_t;

extern uint64_t alloc_size;

extern heap_tag_t * head_tag;
extern heap_tag_t * tail_tag;

void heap_check(void);

void heap_overview(void);

#define FANCY_HEAP_CHECK() \
    do { \
    debug_print("Heap check on line 0x"); \
    debug_print_hex(__LINE__); \
    debug_print(" of file "); \
    debug_print(__FILE_NAME__); \
    debug_print("\n"); \
    heap_check(); \
    } \
    while (0);

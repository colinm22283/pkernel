#pragma once

#include <stdint.h>
#include <stdbool.h>

#define HEAP_DEBUG

#define HEAP_INITIAL_SIZE (0x20000)

typedef struct {
    bool next_reserved;
    uint64_t prev_size;
    uint64_t next_size;

#ifdef HEAP_DEBUG
    const char * name;
#endif
} heap_tag_t;

extern uint64_t alloc_size;

extern heap_tag_t * head_tag;
extern heap_tag_t * tail_tag;

bool heap_check(void);

void heap_overview(void);


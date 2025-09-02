#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct valloc_node_s {
    bool is_free;
    void * start_addr;
    uint64_t size_pages;

    struct valloc_node_s * next;
    struct valloc_node_s * prev;
} valloc_node_t;

typedef struct {
    valloc_node_t head, tail;
} valloc_t;

void valloc_init(valloc_t * valloc);
void valloc_free(valloc_t * valloc);

void * valloc_alloc(valloc_t * valloc, uint64_t size);
void * valloc_reserve(valloc_t * valloc, void * vaddr, uint64_t size);
bool valloc_release(valloc_t * valloc, void * vaddr);

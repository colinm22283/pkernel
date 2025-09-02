#pragma once

#include <stdint.h>

void heap_init(void);

void * heap_alloc(uint64_t size_bytes);
void heap_free(void * alloc);
void * heap_realloc(void * alloc, uint64_t size_bytes);

uint64_t heap_total(void);
uint64_t heap_usage(void);
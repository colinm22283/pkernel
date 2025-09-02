#pragma once

#include <stdint.h>

typedef struct {
    char * name;

    uint64_t capacity;
    char * buffer;
    volatile uint64_t begin;
    volatile uint64_t end;
} interface_fifo_t;

void interface_fifo_init(interface_fifo_t * fifo, const char * name);
void interface_fifo_free(interface_fifo_t * fifo);

void interface_fifo_write(interface_fifo_t * fifo, const char * data, uint64_t size);
uint64_t interface_fifo_read(interface_fifo_t * fifo, char * data, uint64_t size);
void interface_fifo_read_blocking(interface_fifo_t * fifo, char * data, uint64_t size);

void interface_fifo_clear(interface_fifo_t * fifo);
uint64_t interface_fifo_consume(interface_fifo_t * fifo, uint64_t count);
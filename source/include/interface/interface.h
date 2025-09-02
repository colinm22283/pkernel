#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <interface/fifo.h>

typedef struct {
    char * name;

    bool (* method)();
} interface_method_t;

typedef struct {
    bool protected;

    char * name;

    uint64_t method_count, method_capacity;
    interface_method_t * methods;

    uint64_t fifo_count, fifo_capacity;
    interface_fifo_t * fifos;

    uint64_t cbuf_count, cbuf_capacity;

} interface_t;

void interface_init(interface_t * interface, const char * name, bool protected);
void interface_free(interface_t * interface);

void interface_add_method(interface_t * interface, const char * name, bool (* method)());
bool (* interface_lookup_method(interface_t * interface, const char * name))();

void interface_add_fifo(interface_t * interface, const char * name);
interface_fifo_t * interface_lookup_fifo(interface_t * interface, const char * name);
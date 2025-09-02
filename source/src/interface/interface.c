#include <stddef.h>

#include <interface/interface.h>

#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/string/strcmp.h>

void interface_init(interface_t * interface, const char * name, bool protected) {
    interface->name = heap_alloc(strlen(name) + 1);
    strcpy(interface->name, name);

    interface->method_capacity = 1;
    interface->method_count = 0;
    interface->methods = heap_alloc(interface->method_capacity * sizeof(interface_method_t));

    interface->fifo_capacity = 1;
    interface->fifo_count = 0;
    interface->fifos = heap_alloc(interface->fifo_capacity * sizeof(interface_fifo_t));

    interface->protected = protected;
}

void interface_free(interface_t * interface) {
    heap_free(interface->name);

    for (uint64_t i = 0; i < interface->method_count; i++) {
        heap_free(interface->methods[i].name);
    }
    heap_free(interface->methods);

    for (uint64_t i = 0; i < interface->fifo_count; i++) {
        interface_fifo_free(&interface->fifos[i]);
    }
    heap_free(interface->fifos);
}

void interface_add_method(interface_t * interface, const char * name, bool (* method)()) {
    interface->methods[interface->method_count].name = heap_alloc(strlen(name) + 1);
    strcpy(interface->methods[interface->method_count].name, name);
    interface->methods[interface->method_count].method = method;

    interface->method_count++;

    if (interface->method_count == interface->method_capacity) {
        interface->method_capacity *= 2;

        interface->methods = heap_realloc(interface->methods, interface->method_capacity * sizeof(interface_method_t));
    }
}

bool (* interface_lookup_method(interface_t * interface, const char * name))() {
    for (uint64_t i = 0; i < interface->method_count; i++) {
        if (strcmp(interface->methods[i].name, name) == 0) {
            return interface->methods[i].method;
        }
    }

    return NULL;
}

void interface_add_fifo(interface_t * interface, const char * name) {
    interface_fifo_t * fifo = &interface->fifos[interface->fifo_count++];

    interface_fifo_init(fifo, name);

    if (interface->fifo_count == interface->fifo_capacity) {
        interface->fifo_capacity *= 2;

        interface->fifos = heap_realloc(interface->fifos, interface->fifo_capacity * sizeof(interface_fifo_t));
    }
}

interface_fifo_t * interface_lookup_fifo(interface_t * interface, const char * name) {
    for (uint64_t i = 0; i < interface->fifo_count; i++) {
        if (strcmp(interface->fifos[i].name, name) == 0) {
            return &interface->fifos[i];
        }
    }

    return NULL;
}
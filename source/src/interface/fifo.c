#include <interface/fifo.h>

#include <util/heap/heap.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#define INTERFACE_FIFO_INITIAL_CAPACITY (5)

void interface_fifo_init(interface_fifo_t * fifo, const char * name) {
    fifo->name = heap_alloc(strlen(name) + 1);
    strcpy(fifo->name, name);

    fifo->capacity = INTERFACE_FIFO_INITIAL_CAPACITY;
    fifo->buffer = heap_alloc(fifo->capacity);

    fifo->begin = 0;
    fifo->end = 0;
}

void interface_fifo_free(interface_fifo_t * fifo) {
    heap_free(fifo->name);
    heap_free(fifo->buffer);
}

void interface_fifo_write(interface_fifo_t * fifo, const char * data, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        uint64_t next = (fifo->end + 1) % fifo->capacity;

        if (next == fifo->begin) {
            uint64_t cap = fifo->capacity;
            fifo->capacity *= 2;
            fifo->buffer = heap_realloc(fifo->buffer, fifo->capacity);

            if (fifo->begin > fifo->end) {
                for (uint64_t j = fifo->begin; j < cap; j++) {
                    fifo->buffer[j + cap] = fifo->buffer[j];
                }

                fifo->begin += cap;
            }

            fifo->buffer[fifo->end] = data[i];
            fifo->end = (fifo->end + 1) % fifo->capacity;
        }
        else {
            fifo->buffer[fifo->end] = data[i];

            fifo->end = next;
        }
    }
}

uint64_t interface_fifo_read(interface_fifo_t * fifo, char * data, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        if (fifo->begin == fifo->end) return i;

        data[i] = fifo->buffer[fifo->begin];

        fifo->begin = (fifo->begin + 1) % fifo->capacity;
    }

    return size;
}

void interface_fifo_read_blocking(interface_fifo_t * fifo, char * data, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        while (fifo->begin == fifo->end);

        data[i] = fifo->buffer[fifo->begin];

        fifo->begin = (fifo->begin + 1) % fifo->capacity;
    }
}

void interface_fifo_clear(interface_fifo_t * fifo) {
    fifo->begin = fifo->end;
}

uint64_t interface_fifo_consume(interface_fifo_t * fifo, uint64_t count) {
    uint64_t i;

    for (i = 0; i < count && fifo->begin != fifo->end; i++) fifo->begin = (fifo->begin + 1) % fifo->capacity;

    return i;
}
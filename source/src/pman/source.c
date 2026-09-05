#include <stddef.h>

#include <pman/types.h>

#include <util/heap/heap.h>

static inline pman_source_t * pman_source_init(void) {
    pman_source_t * source = heap_alloc(sizeof(pman_source_t));

    source->references = 1;

    source->file        = NULL;
    source->file_offset = 0;

    source->initialized = false;
    source->dirty       = false;
    source->evicted     = false;

    return source;
}

static inline void pman_source_free(pman_source_t * source) {
    heap_free(source);
}

pman_source_t * pman_source_init_file(file_t * file, size_t offset) {
    pman_source_t * source = pman_source_init();

    source->file        = file;
    source->file_offset = offset;
    
    source->initialized = true;
    source->dirty       = false;
    source->evicted     = true;

    return source;
}

pman_source_t * pman_source_init_anon(void) {
    pman_source_t * source = pman_source_init();

    source->file        = NULL;
    
    source->initialized = false;
    source->dirty       = false;
    source->evicted     = false;

    return source;
}

void pman_source_add_ref(pman_source_t * source) {
    source->references++;
}

void pman_source_sub_ref(pman_source_t * source) {
    source->references--;

    if (source->references == 0) {
        pman_source_free(source);
    }
}


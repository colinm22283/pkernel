#include <pman/types.h>

#include <util/heap/heap.h>

static inline pman_source_t * pman_source_init(void) {
    pman_source_t * source = heap_alloc(sizeof(pman_source_t));

    source->references = 1;

    source->file = NULL;

    source->palloc = NULL;
    source->mapping = NULL;

    return source;
}

static inline void pman_source_free(

pman_source_t * pman_source_init_file(file_t * file, size_t offset) {
    pman_source_t * source = pman_source_init();

    source->file        = file;
    source->file_offset = offset;
    source->dirty       = false;

    return source;
}

pman_source_t * pman_source_init_anon(void) {
    pman_source_t * source = pman_source_init();

    return source;
}

void pman_source_deref(pman_source_t * source) {
}


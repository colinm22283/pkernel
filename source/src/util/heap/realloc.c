#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <util/memory/memcpy.h>

void * heap_realloc(void * alloc, uint64_t size_bytes) {
    heap_tag_t * tag = ((heap_tag_t *) alloc) - 1;

    void * new_buffer = heap_alloc(size_bytes);
    memcpy(new_buffer, alloc, tag->next_size);

    heap_free(alloc);

    return new_buffer;
}
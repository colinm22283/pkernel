#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <util/memory/memcpy.h>

void * heap_realloc(void * alloc, uint64_t size_bytes) {
    heap_tag_t * tag = ((heap_tag_t *) alloc) - 1;

    void * new_buffer;
#ifdef HEAP_DEBUG
    new_buffer = heap_alloc_debug(size_bytes, tag->name);
#else
    new_buffer = heap_alloc(size_bytes);
#endif

    if (size_bytes >= tag->next_size) memcpy(new_buffer, alloc, tag->next_size);
    else memcpy(new_buffer, alloc, size_bytes);

    heap_free(alloc);

    return new_buffer;
}

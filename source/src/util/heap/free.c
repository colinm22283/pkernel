#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <entry_error.h>

#include <sys/debug/print.h>

void heap_free(void * alloc) {
    heap_tag_t * tag = ((heap_tag_t *) alloc) - 1;

#ifdef HEAP_DEBUG
    const char * name = tag->name;
#endif

    alloc_size -= tag->next_size;

    if (!tag->next_reserved) kernel_entry_error(KERNEL_ENTRY_ERROR_HEAP_FREE_FAILURE);

    heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) tag + tag->next_size + sizeof(heap_tag_t));

    if (tag->next_size != next_tag->prev_size) {
        kernel_entry_error(KERNEL_ENTRY_ERROR_HEAP_FREE_FAILURE);
    }

    if (!next_tag->next_reserved && next_tag->next_size != 0) {
        tag->next_size += next_tag->next_size + sizeof(heap_tag_t);

        if (next_tag->next_size != 0) {
            heap_tag_t * next_next_tag = (heap_tag_t *) ((intptr_t) next_tag + next_tag->next_size + sizeof(heap_tag_t));
            next_next_tag->prev_size = tag->next_size;

            next_tag = next_next_tag;
        }
    }

    next_tag->prev_size = tag->next_size;

    if (tag->prev_size != 0) {
        heap_tag_t * prev_tag = (heap_tag_t *) ((intptr_t) tag - tag->prev_size - sizeof(heap_tag_t));
        if (!prev_tag->next_reserved) {
            prev_tag->next_size += tag->next_size + sizeof(heap_tag_t);

            next_tag->prev_size = prev_tag->next_size;

            return;
        }
    }

    tag->next_reserved = false;

#ifdef HEAP_DEBUG
    debug_print("FREE: ");
    debug_print(name);
    debug_print("\n");
#endif
}

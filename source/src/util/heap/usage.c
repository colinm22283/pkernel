#include <util/heap/heap.h>
#include <util/heap/internal.h>

uint64_t heap_total(void) {
    return HEAP_INITIAL_SIZE;
}
uint64_t heap_usage(void) {
    uint64_t used = 0;

    heap_tag_t * cur_tag = head_tag;

    while (cur_tag != tail_tag) {
        if (cur_tag->next_reserved) {
            used += cur_tag->next_size;
        }

        cur_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));
    }

    return used;
}
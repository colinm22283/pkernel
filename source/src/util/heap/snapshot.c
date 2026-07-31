#include <util/heap/heap.h>
#include <util/heap/internal.h>

#include <util/string/strcmp.h>

#include <debug/printf.h>

#include <config/heap.h>

typedef struct {
    void * addr;
    const char * name;
} heap_snapshot_t[1024];

void heap_take_snapshot(void) {
    heap_tag_t * cur_tag = head_tag;

    while (cur_tag->next_size != 0) {
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));

#ifdef HEAP_DEBUG
        cur_tag->flags = 1;
#endif

        cur_tag = next_tag;
    }
}

void heap_diff_snapshot(void) {
    heap_tag_t * cur_tag = head_tag;

    while (cur_tag->next_size != 0) {
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));

#ifdef HEAP_DEBUG
        if (cur_tag->next_reserved && cur_tag->flags == 0) {
            printf("Get a load of this guy %i: %s, %i\n", cur_tag->flags, cur_tag->name, cur_tag->next_size);
        }
#endif

        cur_tag = next_tag;
    }
}


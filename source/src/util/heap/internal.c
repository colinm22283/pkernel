#include <util/heap/internal.h>
#include <util/heap/heap.h>

#include <sys/debug/print.h>

#include "sys/panic.h"

uint64_t alloc_size;

heap_tag_t * head_tag;
heap_tag_t * tail_tag;

void heap_check(void) {
    heap_tag_t * cur_tag = head_tag;

    while (cur_tag->next_size != 0) {
        heap_tag_t * prev_tag = (heap_tag_t *) ((intptr_t) cur_tag - cur_tag->prev_size - sizeof(heap_tag_t));
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));

        if (cur_tag->next_reserved && cur_tag->next_size > 0x200) {
            panic3(
                "Heap tag size very large",
                "Prev Alloc", (intptr_t) (prev_tag + 1),
                "Current Alloc", (intptr_t) (cur_tag + 1),
                "Next Alloc", (intptr_t) (next_tag + 1)
            );
        }

        if (cur_tag->next_size != next_tag->prev_size) {
            panic3(
                "Heap tag size mismatch",
                "Prev Alloc", (intptr_t) (prev_tag + 1),
                "Current Alloc", (intptr_t) (cur_tag + 1),
                "Next Alloc", (intptr_t) (next_tag + 1)
            );
        }

        cur_tag = next_tag;
    }

    if (cur_tag != tail_tag) {
        panic1("End tag is not the tail tag", "addr", (intptr_t) cur_tag);
    }
}

void heap_overview(void) {
#ifdef HEAP_DEBUG
    heap_tag_t * cur_tag = head_tag;

    debug_print("\n\n----------\n\n");

    while (cur_tag != tail_tag) {
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));

        if (cur_tag->next_reserved) {
            debug_print("0x");
            debug_print_hex(cur_tag->next_size);
            debug_print(": ");
            debug_print(cur_tag->name);
            debug_print("\n");
        }

        cur_tag = next_tag;
    }

    debug_print("\n\n----------\n\n");

    debug_print("USED: 0x");
    debug_print_hex(heap_usage());
    debug_print("\n");

    debug_print("\n\n----------\n\n");
#endif
}

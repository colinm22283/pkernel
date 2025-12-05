#include <util/heap/internal.h>

#include <debug/vga_print.h>
#include <sys/debug/print.h>

uint64_t alloc_size;

heap_tag_t * head_tag;
heap_tag_t * tail_tag;

bool heap_check(void) {
    heap_tag_t * cur_tag = head_tag;

    while (cur_tag != tail_tag) {
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) cur_tag + cur_tag->next_size + sizeof(heap_tag_t));

        if (cur_tag->next_size != next_tag->prev_size) {
            vga_print("HEAP SANITY FAILED\n");

            asm volatile ("hlt");
        }

        cur_tag = next_tag;
    }

    return true;
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
#endif
}

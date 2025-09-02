#include <stddef.h>

#include <util/heap/heap.h>
#include <util/heap/internal.h>
#include <util/memory/memset.h>

#include <entry_error.h>

#include <debug/vga_print.h>

void * heap_alloc(uint64_t size_bytes) {
    if (size_bytes == 0) kernel_entry_error(0xA552);

    heap_tag_t * current_tag = head_tag;

    alloc_size += size_bytes;

    while (
        current_tag->next_reserved ||
        (
            current_tag->next_size <= size_bytes + sizeof(heap_tag_t) &&
            current_tag->next_size != size_bytes
        )
    ) {
        current_tag = (heap_tag_t *) ((uint64_t) current_tag + current_tag->next_size + sizeof(heap_tag_t));

        if (current_tag->next_size == 0) {
            // TODO: expand the alloc area

            kernel_entry_error(0xA554);

            asm volatile ("mov $0x1234, %%r8\nmov %0, %%r9\ncli\nhlt" : : "r" (current_tag) : "r8", "r9");

            return NULL;
        }
    }

    if (current_tag->next_size == size_bytes) {
        current_tag->next_reserved = true;
    }
    else {
        heap_tag_t * next_tag = (heap_tag_t *) ((intptr_t) current_tag + current_tag->next_size + sizeof(heap_tag_t));
        heap_tag_t * mid_tag = (heap_tag_t *) ((intptr_t) current_tag + size_bytes + sizeof(heap_tag_t));

        if (current_tag->next_reserved) kernel_entry_error((uint64_t) current_tag);

        if (current_tag->next_size != next_tag->prev_size) kernel_entry_error(KERNEL_ENTRY_ERROR_HEAP_ALLOC_FAILURE);

        uint64_t remaining_bytes = current_tag->next_size - size_bytes - sizeof(heap_tag_t);

        current_tag->next_size = size_bytes;
        current_tag->next_reserved = true;

        mid_tag->prev_size = size_bytes;
        mid_tag->next_size = remaining_bytes;
        mid_tag->next_reserved = false;

        next_tag->prev_size = remaining_bytes;
    }

    memset(current_tag + 1, 0, size_bytes);

    return current_tag + 1;
}

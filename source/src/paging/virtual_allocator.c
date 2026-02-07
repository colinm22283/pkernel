#include <stddef.h>

#include <paging/virtual_allocator.h>

#include <util/heap/heap.h>
#include <util/math/div_up.h>
#include <sys/paging/page_size.h>

#include <entry_error.h>

#include "debug/vga_print.h"

#define VALLOC_REGION_SIZE (0x1000000000000)
#define VALLOC_PAGE_SIZE   (0x1000)

void valloc_init(valloc_t * valloc) {
    valloc_node_t * node = heap_alloc_debug(sizeof(valloc_node_t), "valloc_node_t");

    node->is_free = true;
    node->start_addr = (void *) 0;
    node->size_pages = VALLOC_REGION_SIZE / VALLOC_PAGE_SIZE;

    node->prev = &valloc->head;
    node->next = &valloc->tail;

    valloc->head.is_free = false;
    valloc->tail.is_free = false;

    valloc->head.next = node;
    valloc->head.prev = NULL;
    valloc->tail.next = NULL;
    valloc->tail.prev = node;
}

void valloc_free(valloc_t * valloc) {
    valloc_node_t * node = valloc->head.next;

    while (node != &valloc->tail) {
        valloc_node_t * next = node->next;

        heap_free(node);

        node = next;
    }
}

void * valloc_alloc(valloc_t * valloc, uint64_t size) {
    uint64_t size_pages = DIV_UP(size, VALLOC_PAGE_SIZE);

    valloc_node_t * node = valloc->head.next;

    while (true) {
        if (node == &valloc->tail) {
            // TODO: big error

            kernel_entry_error(0x123);
        }

        if (node->is_free && node->size_pages >= size_pages) break;

        node = node->next;
    }

    if (size_pages == node->size_pages) {
        node->is_free = false;
    }
    else {
        valloc_node_t * new_node = heap_alloc_debug(sizeof(valloc_node_t), "valloc_node_t alloc");

        new_node->is_free = true;
        new_node->start_addr = (void *) ((char *) node->start_addr) + (VALLOC_PAGE_SIZE * size_pages);
        new_node->size_pages = node->size_pages - size_pages;

        new_node->prev = node;
        new_node->next = node->next;

        node->next->prev = new_node;
        node->next = new_node;

        node->is_free = false;
        node->size_pages = size_pages;
    }

    return node->start_addr;
}

void * valloc_reserve(valloc_t * valloc, void * vaddr, uint64_t size) {
    if ((intptr_t) vaddr % VALLOC_PAGE_SIZE != 0) return NULL;

    uint64_t size_pages = DIV_UP(size, VALLOC_PAGE_SIZE);

    intptr_t int_vaddr = (intptr_t) vaddr;
    intptr_t req_end = (intptr_t) vaddr + VALLOC_PAGE_SIZE * (intptr_t) size_pages;

    valloc_node_t * node = valloc->head.next;

    while (true) {
        if (node == &valloc->tail) {
            kernel_entry_error(0x123);
        }

        intptr_t start = (intptr_t) node->start_addr;
        intptr_t end = (intptr_t) node->start_addr + VALLOC_PAGE_SIZE * (intptr_t) node->size_pages;

        if (int_vaddr >= start && int_vaddr < end) break;

        node = node->next;
    }

    if (!node->is_free) {
        return NULL;
    }

    intptr_t start = (intptr_t) node->start_addr;
    intptr_t end = (intptr_t) node->start_addr + VALLOC_PAGE_SIZE * (intptr_t) node->size_pages;

    if (req_end > end) {
        return NULL;
    }

    if (int_vaddr > start) {
        valloc_node_t * new_node = heap_alloc_debug(sizeof(valloc_node_t), "valloc_node_t reserve");

        new_node->is_free = true;
        new_node->start_addr = node->start_addr;
        new_node->size_pages = (int_vaddr - start) / VALLOC_PAGE_SIZE;
        new_node->prev = node->prev;
        new_node->next = node;

        node->prev->next = new_node;
        node->prev = new_node;

        node->start_addr = vaddr;
    }

    if (req_end < end) {
        valloc_node_t * new_node = heap_alloc_debug(sizeof(valloc_node_t), "valloc_node_t reserve");

        new_node->is_free = true;
        new_node->start_addr = node->start_addr + VALLOC_PAGE_SIZE * size_pages;
        new_node->size_pages = (end - req_end) / VALLOC_PAGE_SIZE;
        new_node->prev = node;
        new_node->next = node->next;

        node->next->prev = new_node;
        node->next = new_node;
    }

    node->size_pages = size_pages;
    node->is_free = false;

    return node->start_addr;
}

bool valloc_release(valloc_t * valloc, void * vaddr) { // TODO
    valloc_node_t * node = valloc->head.next;

    while (node != &valloc->tail) {
        if (node->start_addr == vaddr) {
            if (node->next != &valloc->tail && node->next->is_free) {
                node->size_pages += node->next->size_pages;

                valloc_node_t * old_node = node->next;

                node->next = old_node->next;
                node->next->prev = node;

                heap_free(old_node);
            }

            if (node->prev != &valloc->head && node->prev->is_free) {
                node->prev->size_pages += node->size_pages;

                valloc_node_t * old_node = node;
                valloc_node_t * prev_node = node->prev;

                prev_node->next = node->next;
                node->next->prev = prev_node;

                node = prev_node;

                heap_free(old_node);
            }

            node->is_free = true;

            return true;
        }

        node = node->next;
    }

    return false;
}

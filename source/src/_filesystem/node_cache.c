#include <stddef.h>

#include <filesystem/node_cache.h>
#include <filesystem/filesystem.h>

#include <util/heap/heap.h>

#define FILESYSTEM_NODE_CACHE_BUCKETS (512)
#define FILESYSTEM_NODE_CACHE_HASH(x) ((x) % FILESYSTEM_NODE_CACHE_BUCKETS)

typedef struct filesystem_node_cache_entry_s {
    uint64_t references;

    filesystem_node_number_t number;
    filesystem_node_t node;

    struct filesystem_node_cache_entry_s * next;
    struct filesystem_node_cache_entry_s * prev;
} filesystem_node_cache_entry_t;

typedef struct {
    filesystem_node_cache_entry_t head, tail;
} filesystem_node_cache_bucket_t;

filesystem_node_cache_bucket_t buckets[FILESYSTEM_NODE_CACHE_BUCKETS];

bool filesystem_node_cache_init(void) {
    for (uint64_t i = 0; i < FILESYSTEM_NODE_CACHE_BUCKETS; i++) {
        buckets[i].head.next = &buckets[i].tail;
        buckets[i].head.prev = NULL;
        buckets[i].tail.next = NULL;
        buckets[i].tail.prev = &buckets[i].head;
    }

    return true;
}

filesystem_node_t * filesystem_node_cache_load(filesystem_node_number_t node_number) {
    uint64_t hash = FILESYSTEM_NODE_CACHE_HASH(node_number);

    filesystem_node_cache_entry_t * entry = buckets[hash].head.next;
    while (entry != &buckets[hash].tail) {
        if (entry->number == node_number) {
            entry->references++;

            return &entry->node;
        }

        entry = entry->next;
    }

    filesystem_node_cache_entry_t * new_entry = heap_alloc(sizeof(filesystem_node_cache_entry_t));

    new_entry->references = 1;
    new_entry->number = node_number;

    new_entry->next = buckets[hash].head.next;
    new_entry->prev = &buckets[hash].head;
    buckets[hash].head.next->prev = new_entry;
    buckets[hash].head.next = new_entry;

    return &new_entry->node;
}

bool filesystem_node_cache_unload(filesystem_node_number_t node_number) {
    uint64_t hash = FILESYSTEM_NODE_CACHE_HASH(node_number);

    filesystem_node_cache_entry_t * entry = buckets[hash].head.next;
    while (entry != &buckets[hash].tail) {
        if (entry->number == node_number) {
            if (entry->references == 1) {
                entry->next->prev = entry->prev;
                entry->prev->next = entry->next;
                heap_free(entry);
            }
            else entry->references--;

            return true;
        }

        entry = entry->next;
    }

    return false;
}
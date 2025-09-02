#pragma once

#include <stddef.h>

#include <filesystem/directory_entries.h>

#include <util/heap/heap.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/string/strcmp.h>

typedef struct directory_entry_node_s {
    directory_entries_t entries;

    struct directory_entry_node_s * next;
    struct directory_entry_node_s * prev;
} directory_entry_node_t;

directory_entry_node_t directory_entry_head;
directory_entry_node_t directory_entry_tail;

bool directory_entries_init(void) {
    directory_entry_head.next = &directory_entry_tail;
    directory_entry_head.prev = NULL;
    directory_entry_tail.next = NULL;
    directory_entry_tail.prev = &directory_entry_head;

    return true;
}

directory_entries_t * directory_add(struct filesystem_node_s * parent_node, struct filesystem_node_s * self_node) {
    directory_entry_node_t * new_node = heap_alloc(sizeof(directory_entry_node_t));

    new_node->entries.parent_node = parent_node;
    new_node->entries.self_node = self_node;

    new_node->entries.head.next = &new_node->entries.tail;
    new_node->entries.head.prev = NULL;
    new_node->entries.tail.next = NULL;
    new_node->entries.tail.prev = &new_node->entries.head;

    new_node->next = directory_entry_head.next;
    new_node->prev = &directory_entry_head;
    directory_entry_head.next->prev = new_node;
    directory_entry_head.next = new_node;

    return &new_node->entries;
}

bool directory_remove(struct filesystem_node_s * node) {
    // TODO
}

directory_entries_t * directory_lookup(struct filesystem_node_s * lookup_node) {
    directory_entry_node_t * node = directory_entry_head.next;

    while (node != &directory_entry_tail) {
        if (node->entries.node == lookup_node) return &node->entries;

        node = node->next;
    }

    return NULL;
}

void directory_entries_add(directory_entries_t * entries, const char * name, struct filesystem_node_s * node) {
    directory_entry_t * new_entry = heap_alloc(sizeof(directory_entry_t));

    new_entry->name = heap_alloc(strlen(name) + 1);
    strcpy(new_entry->name, name);
    new_entry->node = node;

    new_entry->next = entries->head.next;
    new_entry->prev = &entries->head;
    entries->head.next->prev = new_entry;
    entries->head.next = new_entry;
}

void directory_entries_remove(directory_entries_t * entries, const char * name) {
    // TODO
}

struct filesystem_node_s * directory_entries_lookup(directory_entries_t * entries, const char * name) {
    directory_entry_t * node = entries->head.next;

    while (node != &entries->tail) {
        if (strcmp(node->name, name) == 0) {
            return node->node;
        }

        node = node->next;
    }

    return NULL;
}

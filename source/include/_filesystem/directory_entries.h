#pragma once

#include <stdbool.h>

typedef struct directory_entry_s {
    char * name;
    struct filesystem_node_s * node;

    struct directory_entry_s * next;
    struct directory_entry_s * prev;
} directory_entry_t;

typedef struct {
    struct filesystem_node_s * node;

    struct filesystem_node_s * parent_node;
    struct filesystem_node_s * self_node;

    directory_entry_t head, tail;
} directory_entries_t;

bool directory_entries_init(void);

directory_entries_t * directory_add(struct filesystem_node_s * parent_node, struct filesystem_node_s * self_node);
bool directory_remove(struct filesystem_node_s * node);
directory_entries_t * directory_lookup(struct filesystem_node_s * node);

void directory_entries_add(directory_entries_t * entries, const char * name, struct filesystem_node_s * node);
void directory_entries_remove(directory_entries_t * entries, const char * name);
struct filesystem_node_s * directory_entries_lookup(directory_entries_t * entries, const char * name);
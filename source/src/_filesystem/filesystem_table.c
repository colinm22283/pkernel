#include <stddef.h>

#include <filesystem/filesystem_table.h>
#include <filesystem/superblock.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>
#include <util/string/strcpy.h>
#include <util/string/strlen.h>
#include <util/string/strcmp.h>

filesystem_table_entry_t filesystem_table_head, filesystem_table_tail;

bool filesystem_table_init(void) {
    filesystem_table_head.next = &filesystem_table_tail;
    filesystem_table_head.prev = NULL;
    filesystem_table_tail.next = NULL;
    filesystem_table_tail.prev = &filesystem_table_head;

    return true;
}


filesystem_table_entry_t * filesystem_table_push(const char * name, filesystem_table_mount_t mount, superblock_operations_t * superblock_operations) {
    filesystem_table_entry_t * new_entry = heap_alloc(sizeof(filesystem_table_entry_t));

    new_entry->name = heap_alloc(strlen(name) + 1);
    strcpy(new_entry->name, name);
    memcpy(&new_entry->superblock_operations, superblock_operations, sizeof(superblock_operations_t));
    new_entry->mount = mount;

    new_entry->prev = &filesystem_table_head;
    new_entry->next = filesystem_table_head.next;

    filesystem_table_head.next->prev = new_entry;
    filesystem_table_head.next = new_entry;

    return new_entry;
}

bool filesystem_table_remove(filesystem_table_entry_t * entry) {
    return false;
}

filesystem_table_entry_t * filesystem_table_lookup(const char * name) {
    filesystem_table_entry_t * entry = filesystem_table_head.next;

    while (entry != &filesystem_table_tail) {
        if (strcmp(entry->name, name) == 0) return entry;

        entry = entry->next;
    }

    return NULL;
}
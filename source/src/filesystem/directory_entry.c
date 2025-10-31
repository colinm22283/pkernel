#include <stddef.h>

#include <filesystem/directory_entry.h>
#include <filesystem/node.h>
#include <filesystem/filesystem.h>

#include <device/device.h>

#include <util/string/strcmp.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/heap/heap.h>

#include <debug/vga_print.h>
#include <util/heap/internal.h>

#include <sys/halt.h>

fs_directory_entry_t * fs_directory_entry_create(fs_file_type_t type, fs_directory_entry_t * parent, fs_directory_entry_node_t * parent_node) {
    fs_directory_entry_t * directory_entry = heap_alloc(sizeof(fs_directory_entry_t));

    directory_entry->head.next = &directory_entry->tail;
    directory_entry->head.prev = NULL;
    directory_entry->tail.next = NULL;
    directory_entry->tail.prev = &directory_entry->head;

    directory_entry->parent = parent;
    directory_entry->parent_node = parent_node;

    if (parent != NULL) directory_entry->superblock = parent->superblock;

    directory_entry->type = type;
    directory_entry->node = NULL;
    directory_entry->pipe = NULL;
    directory_entry->socket = NULL;
    directory_entry->mounted_fs = NULL;
    directory_entry->references = 1;


    return directory_entry;
}

fs_directory_entry_node_t * fs_directory_entry_add_entry(fs_directory_entry_t * directory_entry, const char * name) {
    for (
        fs_directory_entry_node_t * node = directory_entry->head.next;
        node != &directory_entry->tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) return NULL;
    }

    fs_directory_entry_node_t * new_node = heap_alloc(sizeof(fs_directory_entry_node_t));

    new_node->name = heap_alloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->dirent = NULL;

    new_node->next = directory_entry->head.next;
    new_node->prev = &directory_entry->head;
    directory_entry->head.next->prev = new_node;
    directory_entry->head.next = new_node;

    return new_node;
}

fs_directory_entry_t * fs_directory_entry_enter(fs_directory_entry_t * directory_entry, const char * name) {
    for (
        fs_directory_entry_node_t * node = directory_entry->head.next;
        node != &directory_entry->tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) {
            return fs_directory_node_enter(directory_entry, node);
        }
    }

    return NULL;
}

fs_directory_entry_t * fs_directory_node_enter(fs_directory_entry_t * dirent, fs_directory_entry_node_t * node) {
    if (node->dirent == NULL) {
        fs_directory_entry_add_reference(dirent);

        fs_node_t * new_node = dirent->superblock->superblock_ops->alloc_node(dirent->superblock);

        dirent->superblock->superblock_ops->lookup(dirent, node, new_node);

        dirent->superblock->superblock_ops->list(node->dirent);
    }
    else {
        fs_directory_entry_add_reference(node->dirent);
    }

    return node->dirent;
}

void fs_directory_entry_release(fs_directory_entry_t * directory_entry) {
    if (directory_entry->references == 0) {
        vga_print("AAAAAA\n");
        asm volatile ("hlt");
    }

    directory_entry->references--;

    if (directory_entry->references == 0) {
        if (directory_entry->parent != NULL) {
            directory_entry->parent_node->dirent = NULL;

            fs_directory_entry_release(directory_entry->parent);
        }

        fs_node_release(directory_entry);

        {
            fs_directory_entry_node_t * node = directory_entry->head.next;

            while (node != &directory_entry->tail) {
                fs_directory_entry_node_t * next = node->next;

                heap_free(node->name);
                heap_free(node);

                node = next;
            }
        }

        heap_free(directory_entry);
    }
}

void fs_directory_entry_add_reference(fs_directory_entry_t * directory_entry) {
    directory_entry->references++;
}


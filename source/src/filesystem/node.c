#include <stddef.h>

#include <filesystem/node.h>
#include <filesystem/pipe.h>

#include <util/heap/heap.h>

#include "debug/vga_print.h"
#include "filesystem/superblock.h"

void fs_node_init(fs_node_t * node) {
    node->references = 1;
    node->size = 0;
}

void fs_node_add_reference(fs_node_t * node) {
    node->references++;
}

void fs_node_release(fs_directory_entry_t * dirent) {
    dirent->node->references--;

    if (dirent->node->references == 0) {
        dirent->superblock->superblock_ops->free_node(dirent->superblock, dirent->node);
    }
}

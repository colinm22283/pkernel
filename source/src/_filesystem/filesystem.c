#include <stddef.h>
#include <stdbool.h>

#include <filesystem/filesystem.h>
#include <filesystem/node_cache.h>
#include <filesystem/node_number_allocator.h>
#include <filesystem/filesystem_table.h>
#include <filesystem/directory_entries.h>

#include <paging/virtual_allocator.h>

#include <util/heap/heap.h>
#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/memory/memcpy.h>
#include <util/math/div_up.h>

#include <entry_error.h>

filesystem_node_t filesystem_root_node = {
    .type = NT_DIRECTORY,
    .superblock = NULL,
    .operations = NULL,
};

bool filesystem_init(void) {
    if (!node_number_allocator_init()) return false;
    if (!filesystem_node_cache_init()) return false;
    if (!filesystem_table_init()) return false;
    if (!directory_entries_init()) return false;

    filesystem_root_node.directory_entries = directory_add(&filesystem_root_node, &filesystem_root_node);

    return true;
}

bool filesystem_mount(filesystem_node_t * mount_point, const char * filesystem_name, struct device_s * device) {
    if (mount_point->type != NT_DIRECTORY) return false;

    filesystem_table_entry_t * filesystem_entry = filesystem_table_lookup(filesystem_name);
    if (filesystem_entry == NULL) return false;

    superblock_t * superblock = heap_alloc(sizeof(superblock_t));
    superblock->operations = filesystem_entry->superblock_operations;

    mount_point->superblock = superblock;

    if (!filesystem_entry->mount(superblock, mount_point, device)) {
        heap_free(superblock);

        return false;
    }

    // TODO: store mounts

    return true;
}

filesystem_node_t * filesystem_make_node(filesystem_node_t * parent, filesystem_node_type_t type, const char * name) {
    if (parent->type != NT_DIRECTORY) return NULL;

    filesystem_node_t * new_node = parent->superblock->operations.create_node(parent->superblock);
    new_node->type = type;

    filesystem_mapping_registry_init(&new_node->mapping_registry);

    new_node->directory_entries = directory_add(parent, new_node);

    if (!parent->operations->link(parent, new_node, name)) {

        asm volatile ("hlt");
        parent->superblock->operations.delete_node(parent->superblock, new_node);

        return NULL;
    }

    return new_node;
}

filesystem_node_t * filesystem_find(filesystem_node_t * parent, const char * path) {
    uint64_t start = 0, end = 0;
    filesystem_node_t * node = parent;

    while (true) {
        while (path[end] != '/' && path[end] != '\0') end++;

        if (node->type != NT_DIRECTORY) return NULL;

        char substr[end - start + 1];
        memcpy(substr, path + start, end - start);
        substr[end - start] = '\0';

        filesystem_node_t * new_node = directory_entries_lookup(node->directory_entries, substr);
        if (new_node == NULL) return NULL;

        node = new_node;

        if (path[end] == '\0') break;

        start = ++end;
    }

    return node;
}

uint64_t filesystem_node_write(filesystem_node_t * node, const char * buffer, uint64_t size, uint64_t offset) {
    return node->operations->write(node, buffer, size, offset);
}

uint64_t filesystem_node_read(filesystem_node_t * node, char * buffer, uint64_t size, uint64_t offset) {
    return node->operations->read(node, buffer, size, offset);
}

void * filesystem_node_map(filesystem_node_t * node, pml4t64_t * pml4t, void * map_addr, uint64_t size, uint64_t offset) {
    void ** private = filesystem_mapping_registry_add(&node->mapping_registry, pml4t, map_addr);

    void * vaddr;

    if (map_addr == NULL) vaddr = paging_valloc_alloc(DIV_UP(size, 0x1000));
    else vaddr = map_addr;

    if (!node->operations->map(node, private, pml4t, vaddr, size, offset)) {
        paging_valloc_free(vaddr, DIV_UP(size, 0x1000));

        return NULL;
    }

    return vaddr;
}

bool filesystem_node_unmap(filesystem_node_t * node, pml4t64_t * pml4t, void * map_addr) {

}

filesystem_node_t * filesystem_root(void) {
    return &filesystem_root_node;
}

#include <stddef.h>

#include <filesystem/ramfs/ramfs.h>

#include <util/heap/heap.h>
#include <util/memory/memcpy.h>

filesystem_node_operations_t ramfs_node_operations = {
    .write = ramfs_write,
    .read = ramfs_read,
    .link = ramfs_link,
};

uint64_t ramfs_write(filesystem_node_t * node, const char * buffer, uint64_t size, uint64_t offset) {
    if (node->type != NT_REGULAR) return 0;

    regular_file_data_t * data = node->private;

    if (node->private == NULL) {
        node->private = heap_alloc(sizeof(regular_file_data_t));
        data = node->private;

        data->data = NULL;
        data->size = 0;
    }

    uint64_t end = offset + size;

    if (end > data->size) {
        if (data->size == 0) data->data = heap_alloc(end);
        else data->data = heap_realloc(data->data, end);

        data->size = end;
    }

    memcpy(data->data + offset, buffer, size);

    return size;
}

uint64_t ramfs_read(filesystem_node_t * node, char * buffer, uint64_t size, uint64_t offset) {
    if (node->type != NT_REGULAR) return 0;
    if (node->private == NULL) return 0;

    regular_file_data_t * data = node->private;

    uint64_t end = offset + size;
    if (data->size > end) end = data->size;

    memcpy(buffer, data->data + offset, end - offset);

    return end - offset;
}

bool ramfs_link(filesystem_node_t * node, filesystem_node_t * subnode, const char * name) {
    if (node->type != NT_DIRECTORY) return false;

    directory_entries_add(node->directory_entries, name, subnode);

    return true;
}
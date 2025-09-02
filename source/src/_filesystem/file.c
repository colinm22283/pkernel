#include <stddef.h>

#include <filesystem/file.h>

#include <util/heap/heap.h>

typedef struct file_node_s {
    file_t file;

    struct file_node_s * next;
    struct file_node_s * prev;
} file_node_t;

file_node_t file_head, file_tail;

bool filesystem_file_init(void) {
    file_head.next = &file_tail;
    file_head.prev = NULL;
    file_tail.next = NULL;
    file_tail.prev = &file_head;

    return true;
}

file_t * filesystem_open_file(filesystem_node_t * node, file_open_mode_t mode) {
    file_node_t * new_node = heap_alloc(sizeof(file_node_t));

    new_node->file.node = node;
    new_node->file.position = 0;
    new_node->file.mode = mode;

    new_node->next = file_head.next;
    new_node->prev = &file_head;
    file_head.next->prev = new_node;
    file_head.next = new_node;

    return &new_node->file;
}

bool filesystem_close_file(file_t * node) {
    return false; // TODO
}

int64_t filesystem_write_file(file_t * file, const char * buffer, uint64_t size) {
    if (!(file->mode & FOM_WRITE)) return FILE_ERROR_NO_PERMISSIONS;

    int64_t result = (int64_t) filesystem_node_write(file->node, buffer, size, file->position);

    file->position += size;

    return result;
}

int64_t filesystem_read_file(file_t * file, char * buffer, uint64_t size) {
    if (!(file->mode & FOM_READ)) return FILE_ERROR_NO_PERMISSIONS;

    int64_t result = (int64_t) filesystem_node_read(file->node, buffer, size, file->position);

    file->position += size;

    return result;
}

void filesystem_seek_file(file_t * file, int64_t offset, file_seek_origin_t origin) {
    switch (origin) {
        case FSO_BEGIN: file->position = offset; break;
        case FSO_END: file->position = file->node->size + offset; break;
        case FSO_CURRENT: file->position += offset; break;
    }
}

void * filesystem_map_file(file_t * file, pml4t64_t * pml4t, void * address, uint64_t size, uint64_t offset) {
    if (!(file->mode & FOM_READ) || !(file->mode & FOM_WRITE)) return NULL;

    return filesystem_node_map(file->node, pml4t, address, size, offset);
}
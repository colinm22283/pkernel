#include <stddef.h>

#include <filesystem/pipe.h>
#include <filesystem/directory_entry.h>

#include <util/heap/heap.h>

static inline bool pipe_push(pipe_t * pipe, char c) {
    if (pipe->size == PIPE_BUFFER_SIZE) return false;

    pipe->buffer[(pipe->start + pipe->size) % PIPE_BUFFER_SIZE] = c;

    pipe->size++;

    return true;
}

static inline bool pipe_pop(pipe_t * pipe, char * c) {
    if (pipe->size == 0) return false;

    *c = pipe->buffer[pipe->start];
    pipe->start++;
    pipe->size--;

    return true;
}

pipe_t * pipe_init(void) {
    pipe_t * pipe = heap_alloc_debug(sizeof(pipe_t), "pipe");

    pipe->buffer_alloc = pman_context_add_alloc(
        pman_kernel_context(),
        0,
        NULL,
        PIPE_BUFFER_SIZE
    );

    pipe->start  = 0;
    pipe->size   = 0;
    pipe->buffer = pipe->buffer_alloc->vaddr;

    return pipe;
}

error_number_t pipe_free(pipe_t * pipe) {
    pman_context_unmap(pipe->buffer_alloc);

    heap_free(pipe);

    return ERROR_OK;
}

error_number_t pipe_read(fs_directory_entry_t * dirent, char * data, fs_size_t size, fs_size_t offset, fs_size_t * read) {
    pipe_t * pipe = dirent->pipe;

    for (fs_size_t i = 0; i < size; i++) {
        if (!pipe_pop(pipe, &data[i])) {
            *read = i;
            return ERROR_OK;
        }
    }

    *read = size;
    return ERROR_OK;
}

error_number_t pipe_write(fs_directory_entry_t * dirent, const char * data, fs_size_t size, fs_size_t offset, fs_size_t * wrote) {
    pipe_t * pipe = dirent->pipe;

    for (fs_size_t i = 0; i < size; i++) {
        if (!pipe_push(pipe, data[i])) {
            *wrote = i;
            return ERROR_OK;
        }
    }

    *wrote = size;
    return ERROR_OK;
}

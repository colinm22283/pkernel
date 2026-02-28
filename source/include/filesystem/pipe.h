#pragma once

#include <paging/manager.h>

#include <filesystem/types.h>

#define PIPE_BUFFER_SIZE (1024)

typedef struct {
    uint64_t references;

    uint64_t start, size;
    char * buffer;

    pman_mapping_t * buffer_alloc;
} pipe_t;

pipe_t * pipe_init(void);
int pipe_free(pipe_t * pipe);

int pipe_read(struct fs_directory_entry_s * dirent, char * data, fs_size_t size, fs_size_t offset, fs_size_t * read);
int pipe_write(struct fs_directory_entry_s * dirent, const char * data, fs_size_t size, fs_size_t offset, fs_size_t * wrote);

#pragma once

#include <stddef.h>
#include <dirent.h>

#include <filesystem/node.h>
#include <filesystem/filesystem.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#include <sys/types.h>

typedef struct {
    size_t references;

    fs_directory_entry_t * dirent;
    int options;

    // file data
    uint64_t offset;

    // directory data
    fs_directory_entry_node_t * current_node;
} fs_file_t;

fs_file_t * file_alloc(void);

int file_init(fs_file_t * file, fs_directory_entry_t * dirent, int options);

int file_clone(fs_file_t * dst, fs_file_t * src);

void file_add_ref(fs_file_t * file);

int64_t file_read(fs_file_t * file, char * buffer, uint64_t size);

int64_t file_write(fs_file_t * file, const char * buffer, uint64_t size);

void * file_map(fs_file_t * file, pman_context_t * context, void * map_addr, uint64_t size, uint64_t offset);

void file_close(fs_file_t * file);

int file_readdir(fs_file_t * file, struct dirent * entries, size_t buffer_size);


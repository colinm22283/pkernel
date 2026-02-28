#pragma once

#include <stddef.h>

#include <filesystem/node.h>
#include <filesystem/filesystem.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#include <sys/types.h>

typedef struct {
    fs_directory_entry_t * dirent;
    open_options_t options;

    // file data
    uint64_t offset;

    // directory data
    fs_directory_entry_node_t * current_node;
} fs_file_t;

int file_init(fs_file_t * file, fs_directory_entry_t * dirent, open_options_t options);

int file_clone(fs_file_t * dst, fs_file_t * src);

int64_t file_read(fs_file_t * file, char * buffer, uint64_t size);

int64_t file_write(fs_file_t * file, const char * buffer, uint64_t size);

void * file_map(fs_file_t * file, pman_context_t * context, void * map_addr, uint64_t size, uint64_t offset);

void file_close(fs_file_t * file);

int64_t file_readdir(fs_file_t * file, directory_entry_t * entries, uint64_t buffer_size);


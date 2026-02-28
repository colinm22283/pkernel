#pragma once

#include <filesystem/file.h>

typedef struct {
    size_t file_capacity;
    fs_file_t ** files;
} file_table_t;

void file_table_init(file_table_t * file_table);
void file_table_free(file_table_t * file_table);

void file_table_clone(file_table_t * dst, file_table_t * src);

int file_table_dup(file_table_t * file_table, fd_t dst, fd_t src);
fd_t file_table_open(file_table_t * file_table, fs_directory_entry_t * node, open_options_t options);
fd_t file_table_openat(file_table_t * file_table, fd_t fd, fs_directory_entry_t * node, open_options_t options);
fs_file_t * file_table_get(file_table_t * file_table, fd_t fd);

int file_table_close(file_table_t * file_table, fd_t fd);

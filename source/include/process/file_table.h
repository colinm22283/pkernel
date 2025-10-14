#pragma once

#include <filesystem/file.h>

typedef struct {
    size_t file_count, file_capacity;
    fs_file_t ** files;
} file_table_t;

void file_table_init(file_table_t * file_table);
void file_table_free(file_table_t * file_table);
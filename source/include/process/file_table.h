#pragma once

#include <filesystem/file.h>

#include <pkos/types.h>

typedef struct {
    int64_t file_count, file_capacity;
    fs_file_t ** files;
} process_file_table_t;

error_number_t process_file_table_init(process_file_table_t * pft);
error_number_t process_file_table_clone(process_file_table_t * dst, process_file_table_t * src);
void process_file_table_free(process_file_table_t * pft);

fd_t process_file_table_set(process_file_table_t * pft, fd_t fd, fs_directory_entry_t * node, open_options_t options);

fd_t process_file_table_open(process_file_table_t * pft, fs_directory_entry_t * node, open_options_t options);
error_number_t process_file_table_close(process_file_table_t * pft, fd_t fd);
fs_file_t * process_file_table_get(process_file_table_t * pft, fd_t fd);
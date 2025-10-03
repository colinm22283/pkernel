#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <filesystem/types.h>
#include <filesystem/directory_entry.h>

#include <error_number.h>

#include <pkos/types.h>

typedef struct fs_node_s {
    bool delete;

    uint64_t references;

    uint64_t size;
} fs_node_t;

void fs_node_init(fs_node_t * node);

void fs_node_add_reference(fs_node_t * node);
void fs_node_release(fs_directory_entry_t * dirent);

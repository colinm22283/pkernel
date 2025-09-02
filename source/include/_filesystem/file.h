#pragma once

#include <stdbool.h>

#include <filesystem/filesystem.h>

enum {
    FILE_ERROR_NO_PERMISSIONS = -256,
};

typedef enum {
    FOM_NULL        = 0b00,
    FOM_READ        = 0b01,
    FOM_WRITE       = 0b10,
    FOM_READ_WRITE  = 0b11,
} file_open_mode_t;

typedef struct {
    filesystem_node_t * node;

    file_open_mode_t mode;

    uint64_t position;
} file_t;

bool filesystem_file_init(void);

file_t * filesystem_open_file(filesystem_node_t * node, file_open_mode_t mode);
bool filesystem_close_file(file_t * node);

typedef enum {
    FSO_BEGIN   = 0,
    FSO_END     = 1,
    FSO_CURRENT = 2,
} file_seek_origin_t;

int64_t filesystem_write_file(file_t * file, const char * buffer, uint64_t size);
int64_t filesystem_read_file(file_t * file, char * buffer, uint64_t size);
void filesystem_seek_file(file_t * file, int64_t offset, file_seek_origin_t origin);
void * filesystem_map_file(file_t * file, pml4t64_t * pml4t, void * address, uint64_t size, uint64_t offset);
void * filesystem_unmap_file(file_t * file, void * address);
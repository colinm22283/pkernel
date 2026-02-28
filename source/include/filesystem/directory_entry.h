#pragma once

#include <stdint.h>

#include <device/device.h>

#include <filesystem/types.h>
#include <filesystem/pipe.h>
#include <filesystem/socket.h>

#include <errno.h>

#include <sys/types.h>

typedef struct fs_directory_entry_node_s {
    struct fs_directory_entry_s * dirent;
    char * name;

    struct fs_directory_entry_node_s * next;
    struct fs_directory_entry_node_s * prev;
} fs_directory_entry_node_t;

typedef struct fs_directory_entry_s {
    struct fs_directory_entry_s * parent;
    struct fs_directory_entry_node_s * parent_node;

    uint64_t references;

    struct fs_superblock_s * superblock;

    fs_file_type_t type;
    struct fs_node_s * node;

    device_t * device;
    pipe_t * pipe;
    socket_t * socket;

    struct fs_filesystem_node_s * mounted_fs;

    fs_directory_entry_node_t head, tail;
} fs_directory_entry_t;

fs_directory_entry_t * fs_directory_entry_create(fs_file_type_t type, fs_directory_entry_t * parent, fs_directory_entry_node_t * parent_node);
fs_directory_entry_node_t * fs_directory_entry_add_entry(fs_directory_entry_t * directory_entry, const char * name);

fs_directory_entry_t * fs_directory_entry_enter(fs_directory_entry_t * directory_entry, const char * name);
fs_directory_entry_t * fs_directory_node_enter(fs_directory_entry_t * dirent, fs_directory_entry_node_t * node);

void fs_directory_entry_release(fs_directory_entry_t * directory_entry);
void fs_directory_entry_add_reference(fs_directory_entry_t * directory_entry);

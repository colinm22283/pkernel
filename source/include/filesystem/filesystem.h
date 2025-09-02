#pragma once

#include <stdbool.h>

#include <filesystem/node.h>
#include <filesystem/superblock.h>

#include <device/device.h>

#include <error_number.h>

typedef error_number_t (* fs_mount_func_t)(fs_superblock_t * superblock);
typedef error_number_t (* fs_unmount_func_t)(fs_superblock_t * superblock);

extern fs_directory_entry_t fs_root;

bool fs_init();

error_number_t fs_register(const char * name, fs_mount_func_t mount, fs_unmount_func_t unmount);
error_number_t fs_unregister(const char * name);

error_number_t fs_mount(const char * name, fs_directory_entry_t * mount_point, device_t * device);
error_number_t fs_unmount(fs_node_t * mount_point);

fs_directory_entry_t * fs_make(fs_directory_entry_t * parent, const char * name, fs_file_type_t type);
fs_directory_entry_t * fs_make_anon(fs_file_type_t type);

fs_directory_entry_t * fs_open_path(fs_directory_entry_t * root, const char * path);
fs_directory_entry_t * fs_make_path(fs_directory_entry_t * root, const char * path, fs_file_type_t type);

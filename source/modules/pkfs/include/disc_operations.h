#pragma once

#include <pkfs.h>

pkfs_directory_t open_filesystem(device_t * device, filesystem_page_address_t root_address);
pkfs_directory_t open_directory(device_t * device, pkfs_directory_t parent, const char * directory_name);
pkfs_file_t open_file(device_t * device, pkfs_directory_t parent, const char * directory_name);

bool get_directory_name(device_t * device, pkfs_directory_t file, char * buffer);
bool get_file_name(device_t * device, pkfs_file_t file, char * buffer);
uint64_t get_file_size(device_t * device, pkfs_file_t file);

bool directory_iterator_init(device_t * device, directory_iterator_t * iterator, pkfs_directory_t directory);
filesystem_directory_entry_type_t directory_iterator_next(device_t * device, directory_iterator_t * iterator, filesystem_page_address_t * handle);

pkfs_file_t create_file(device_t * device);
pkfs_file_t create_dir(device_t * device);

bool link_node(device_t * device, pkfs_directory_t directory, filesystem_page_address_t node, const char * name);

uint64_t read_file(device_t * device, pkfs_file_t file, char * buffer, uint64_t size, uint64_t offset);
uint64_t write_file(device_t * device, pkfs_file_t file, const char * buffer, uint64_t size, uint64_t offset);

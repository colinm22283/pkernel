#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <filesystem/node.h>

#include <sys/paging/pml4t.h>

struct device_s;

bool filesystem_init(void);

bool filesystem_mount(filesystem_node_t * mount_point, const char * filesystem_name, struct device_s * device);
filesystem_node_t * filesystem_make_node(filesystem_node_t * parent, filesystem_node_type_t type, const char * name);

filesystem_node_t * filesystem_find(filesystem_node_t * parent, const char * path);

uint64_t filesystem_node_write(filesystem_node_t * node, const char * buffer, uint64_t size, uint64_t offset);
uint64_t filesystem_node_read(filesystem_node_t * node, char * buffer, uint64_t size, uint64_t offset);
void * filesystem_node_map(filesystem_node_t * node, pml4t64_t * pml4t, void * map_addr, uint64_t size, uint64_t offset);
bool filesystem_node_unmap(filesystem_node_t * node, pml4t64_t * pml4t, void * map_addr);

filesystem_node_t * filesystem_root(void);

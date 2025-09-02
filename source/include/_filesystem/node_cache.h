#pragma once

#include <filesystem/node.h>

bool filesystem_node_cache_init(void);

filesystem_node_t * filesystem_node_cache_load(filesystem_node_number_t node_number);
bool filesystem_node_cache_unload(filesystem_node_number_t node_number);
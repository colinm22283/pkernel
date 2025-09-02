#pragma once

#include <stdint.h>

#include <interface/interface.h>

typedef struct interface_map_node_s {
    interface_t interface;

    uint64_t instance_number;

    struct interface_map_node_s * next;
    struct interface_map_node_s * prev;
} interface_map_node_t;

typedef struct {
    interface_map_node_t head, tail;
} interface_map_t;

extern interface_map_t interface_map;

void interface_map_init(void);
void interface_map_free(void);

interface_map_node_t * interface_map_add(const char * name);
interface_map_node_t * interface_map_add_unprotected(const char * name);

void interface_map_remove(interface_map_node_t * node);

interface_t * interface_map_lookup(const char * name);
interface_t * interface_map_lookup_instance(const char * name, uint64_t instance_number);
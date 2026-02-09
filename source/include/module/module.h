#pragma once

#include <stddef.h>

#include <paging/manager.h>

#include <sysfs/sysfs.h>

typedef error_number_t module_init_t(void);
typedef error_number_t module_free_t(void);

typedef enum {
    MODULE_STATIC,
    MODULE_DYNAMIC,
} module_type_t;

typedef struct module_s {
    char * name;

    size_t dep_count;
    char ** deps;

    module_type_t type;

    module_init_t * init;
    module_free_t * free;

    size_t mapping_count;
    pman_mapping_t ** mappings;

    struct module_s * next;
    struct module_s * prev;
} module_t;

void modules_init(void);

error_number_t module_register_static(const char * module_name, const char ** deps, size_t dep_count, module_init_t * init, module_free_t * free);

error_number_t module_load(const char * name);

error_number_t module_unload(module_t * module);

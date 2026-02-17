#include <module/module.h>

#include <util/heap/heap.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>
#include <util/string/strcmp.h>

#include <config/module.h>

#ifdef MODULE_DEBUG
    #define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_DEBUG_LOGGER("module loader");

module_t loaded_head, loaded_tail;

module_t modules_head, modules_tail;

void modules_init(void) {
    modules_head.next = &modules_tail;
    modules_head.prev = NULL;
    modules_tail.next = NULL;
    modules_tail.prev = &modules_head;
    
    loaded_head.next = &loaded_tail;
    loaded_head.prev = NULL;
    loaded_tail.next = NULL;
    loaded_tail.prev = &loaded_head;
}

int64_t sysfs_read(uint64_t id, char * data, uint64_t size, uint64_t offset) {
    return 0;
}

int64_t sysfs_write(uint64_t id, const char * data, uint64_t size, uint64_t offset) {
    return 0;
}

error_number_t module_register_static(const char * module_name, const char ** deps, size_t dep_count, module_init_t * init, module_free_t * free) {
    DEBUG_LOG(
        DEBUG_PRINT("Register static module \'");
        DEBUG_PRINT(module_name);
        DEBUG_PRINT("\'");
    );

    size_t module_name_len = strlen(module_name);

    module_t * module = heap_alloc(sizeof(module_t));

    module->name = heap_alloc(module_name_len + 1);
    strcpy(module->name, module_name);

    module->dep_count = dep_count;
    if (module->dep_count == 0) {
        module->deps = NULL;
    }
    else {
        module->deps = heap_alloc(module->dep_count * sizeof(char *));
        for (size_t i = 0; i < module->dep_count; i++) {
            module->deps[i] = heap_alloc(strlen(deps[i]) + 1);
            strcpy(module->deps[i], deps[i]);
        }
    }

    module->init = init;
    module->free = free;

    module->loaded = false;

    module->next = modules_head.next;
    module->prev = &modules_head;
    modules_head.next->prev = module;
    modules_head.next = module;

    return ERROR_OK;
}

error_number_t module_load(const char * name) {
    for (module_t * module = loaded_head.next; module != &loaded_tail; module = module->next) {
        if (strcmp(module->name, name) == 0) {
            return ERROR_MOD_LOADED;
        }
    }

    DEBUG_LOG(
        DEBUG_PRINT("Load module \'");
        DEBUG_PRINT(name);
        DEBUG_PRINT("\'");
    );

    for (module_t * module = modules_head.next; module != &modules_tail; module = module->next) {
        if (strcmp(module->name, name) == 0) {
            error_number_t result;

            for (size_t i = 0; i < module->dep_count; i++) {
                result = module_load(module->deps[i]);

                if (result != ERROR_OK && result != ERROR_MOD_LOADED) {
                    debug_print("Failed to load module dependency \"");
                    debug_print(module->deps[i]);
                    debug_print("\"\n");

                    return result;
                }
            }

            result = module->init();
            if (result != ERROR_OK) {
                debug_print("Failed to init module \"");
                debug_print(module->name);
                debug_print("\"\n");
                return result;
            }

            char path[7 + strlen(module->name) + 1];
            strcpy(path, "module/");
            strcpy(path + 7, module->name);

            result = sysfs_add_entry(path, 0, sysfs_read, sysfs_write);
            if (result != ERROR_OK) {
                module->free();

                return result;
            }

            module->loaded = true;

            module->next->prev = module->prev;
            module->prev->next = module->next;

            module->next = loaded_head.next;
            module->prev = &loaded_head;
            loaded_head.next->prev = module;
            loaded_head.next = module;

            return ERROR_OK;
        }
    }

    return ERROR_MOD_NONE;
}

error_number_t module_unload(const char * name) {
    module_t * found_module = NULL;

    for (module_t * module = loaded_head.next; module != &loaded_tail; module = module->next) {
        if (strcmp(module->name, name) == 0) {
            found_module = module;

            break;
        }
    }

    if (found_module == NULL) {
        return ERROR_MOD_NONE;
    }

    DEBUG_LOG(
        DEBUG_PRINT("Unload module \'");
        DEBUG_PRINT(found_module->name);
        DEBUG_PRINT("\'");
    );

    return ERROR_UNIMPLEMENTED;
}

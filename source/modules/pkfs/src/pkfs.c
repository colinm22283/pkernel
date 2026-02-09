#include <stdbool.h>
#include <stddef.h>

#include <pkfs.h>
#include <error_number.h>
#include <mod_defs.h>

#include <filesystem/filesystem.h>

error_number_t init() {
    fs_register("pkfs", &superblock_ops, mount, unmount);

    return ERROR_OK;
}

error_number_t free() {
    fs_unregister("pkfs");

    return ERROR_OK;
}

MODULE_NAME("pkfs");
MODULE_DEPS_NONE();

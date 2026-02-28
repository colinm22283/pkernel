#include <stdbool.h>
#include <stddef.h>

#include <pkfs.h>
#include <errno.h>
#include <mod_defs.h>

#include <filesystem/filesystem.h>

int init() {
    fs_register("pkfs", &superblock_ops, mount, unmount);

    return ERROR_OK;
}

int free() {
    fs_unregister("pkfs");

    return ERROR_OK;
}

MODULE_NAME("pkfs");
MODULE_DEPS_NONE();

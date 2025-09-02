#include <stdbool.h>
#include <stddef.h>

#include <pkfs.h>

#include <filesystem/filesystem.h>

bool init() {
    fs_register("pkfs", mount, unmount);

    return true;
}

bool free() {
    fs_unregister("pkfs");

    return true;
}
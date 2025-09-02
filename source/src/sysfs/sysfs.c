#include <stddef.h>

#include <sysfs/sysfs.h>

#include <filesystem/node.h>
#include <filesystem/file.h>



error_number_t sysfs_init(void) {
    // error_number_t result;
    //
    // fs_directory_entry_t * root_dirent = fs_get_directory_entry(fs_root);
    // fs_node_t * sys_node = fs_directory_entry_enter(root_dirent, "sys");
    //
    // result = fs_mount("sysfs", sys_node, NULL);
    // if (result != ERROR_OK) return result;
    //
    // fs_directory_entry_release(root_dirent);
    //
    return ERROR_OK;
}

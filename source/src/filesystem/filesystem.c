#include <stddef.h>
#include <stdint.h>

#include <filesystem/filesystem.h>

#include <filesystem/pipe.h>

#include <util/string/strcmp.h>
#include <util/string/strcpy.h>
#include <util/string/strlen.h>
#include <util/memory/memcpy.h>
#include <util/heap/heap.h>

#include <error_number.h>
#include <entry_error.h>

#include <debug/vga_print.h>

#include "../../modules/pkfs/include/pkfs.h"
#include "sys/halt.h"

fs_directory_entry_t fs_root;

fs_filesystem_node_t fs_filesystem_head, fs_filesystem_tail;

bool fs_init() {
    fs_root.node = NULL;
    fs_root.parent = NULL;
    fs_root.superblock = NULL;
    fs_root.type = FS_DIRECTORY;
    fs_root.parent_node = NULL;
    fs_root.references = 1;

    fs_filesystem_head.next = &fs_filesystem_tail;
    fs_filesystem_head.prev = NULL;
    fs_filesystem_tail.next = NULL;
    fs_filesystem_tail.prev = &fs_filesystem_head;

    return true;
}

error_number_t fs_register(const char * name, const fs_superblock_ops_t * superblock_ops, fs_mount_func_t mount, fs_unmount_func_t unmount) {
    for (
        fs_filesystem_node_t * node = fs_filesystem_head.next;
        node != &fs_filesystem_tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) {
            return ERROR_FILESYSTEM_EXISTS;
        }
    }

    fs_filesystem_node_t * new_node = heap_alloc(sizeof(fs_filesystem_node_t));

    new_node->name = heap_alloc(strlen(name) + 1);
    strcpy(new_node->name, name);
    new_node->mount_count = 0;
    new_node->superblock_ops = superblock_ops;
    new_node->mount = mount;
    new_node->unmount = unmount;

    new_node->prev = &fs_filesystem_head;
    new_node->next = fs_filesystem_head.next;
    fs_filesystem_head.next->prev = new_node;
    fs_filesystem_head.next = new_node;

    return ERROR_OK;
}

error_number_t fs_unregister(const char * name) {
    for (
        fs_filesystem_node_t * node = fs_filesystem_head.next;
        node != &fs_filesystem_tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) {
            if (node->mount_count > 0) return ERROR_FILESYSTEM_HAS_MOUNT;

            node->prev->next = node->next;
            node->next->prev = node->prev;

            heap_free(node->name);
            heap_free(node);

            return ERROR_OK;
        }
    }

    return ERROR_FILESYSTEM_NOT_FOUND;
}

error_number_t fs_mount_root(const char * name, device_t * device) {
    fs_root.head.next = &fs_root.tail;
    fs_root.head.prev = NULL;
    fs_root.tail.next = NULL;
    fs_root.tail.prev = &fs_root.head;

    for (
        fs_filesystem_node_t * node = fs_filesystem_head.next;
        node != &fs_filesystem_tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) {
            fs_directory_entry_add_reference(&fs_root);

            fs_superblock_t * superblock = superblock_alloc(node->superblock_ops);

            superblock->prev_dirent = NULL;
            superblock->mount_point = &fs_root;
            superblock->device = device;
            superblock->mount_point->superblock = superblock;
            superblock->mount_point->type = FS_DIRECTORY;

            fs_node_t * new_node = node->superblock_ops->alloc_node(superblock);
            superblock->mount_point->node = new_node;

            error_number_t result = node->mount(superblock);
            if (result != ERROR_OK) return result;

            node->superblock_ops->list(superblock->mount_point);
            superblock->mount_point->mounted_fs = node;

            node->mount_count++;

            return ERROR_OK;
        }
    }

    return ERROR_FILESYSTEM_NOT_FOUND;
}

error_number_t fs_mount(const char * name, fs_directory_entry_t * mount_point, device_t * device) {
    if (mount_point->type != FS_DIRECTORY) return ERROR_NOT_DIR;

    for (
        fs_filesystem_node_t * node = fs_filesystem_head.next;
        node != &fs_filesystem_tail;
        node = node->next
    ) {
        if (strcmp(node->name, name) == 0) {
            fs_directory_entry_add_reference(mount_point);

            fs_superblock_t * superblock = superblock_alloc(node->superblock_ops);

            fs_directory_entry_t * new_dirent = fs_directory_entry_create(FS_DIRECTORY, mount_point->parent, mount_point->parent_node);

            superblock->prev_dirent = mount_point;
            superblock->mount_point = new_dirent;
            superblock->device = device;
            superblock->mount_point->superblock = superblock;
            superblock->mount_point->type = FS_DIRECTORY;
            superblock->mount_point->parent_node->dirent = superblock->mount_point;

            fs_node_t * new_node = node->superblock_ops->alloc_node(superblock);
            superblock->mount_point->node = new_node;

            error_number_t result = node->mount(superblock);
            if (result != ERROR_OK) return result;

            node->superblock_ops->list(superblock->mount_point);
            superblock->mount_point->mounted_fs = node;

            node->mount_count++;

            return ERROR_OK;
        }
    }

    return ERROR_FILESYSTEM_NOT_FOUND;
}

error_number_t fs_unmount(fs_directory_entry_t * mount_point) {
     if (mount_point->mounted_fs == NULL) {
         return ERROR_FILESYSTEM_NOT_MOUNTED;
     }

    mount_point->mounted_fs->unmount(mount_point->superblock);

    if (mount_point->parent_node != NULL) mount_point->parent_node->dirent = mount_point->superblock->prev_dirent;

    if (mount_point->superblock->prev_dirent != NULL) fs_directory_entry_release(mount_point->superblock->prev_dirent);
    fs_directory_entry_release(mount_point);

    mount_point->mounted_fs = NULL;
    if (mount_point->parent != NULL) mount_point->superblock = mount_point->parent->superblock;

    return ERROR_OK;
}

fs_directory_entry_t * fs_make(fs_directory_entry_t * parent, const char * name, fs_file_type_t type) {
    fs_node_t * new_node = parent->superblock->superblock_ops->alloc_node(parent->superblock);

    if (new_node == NULL) return NULL;

    fs_directory_entry_node_t * dirent_node = parent->superblock->superblock_ops->create(parent, new_node, name, type);

    if (dirent_node == NULL) return NULL; // TODO: leak

    fs_directory_entry_add_reference(parent);

    return dirent_node->dirent;
}

fs_directory_entry_t * fs_make_anon(fs_file_type_t type) {
    fs_directory_entry_t * new_dirent = fs_directory_entry_create(type, NULL, NULL);

    new_dirent->node = NULL;

    switch (type) {
        case FS_PIPE: {
            new_dirent->pipe = pipe_init();
        } break;
    }

    return new_dirent;
}

fs_directory_entry_t * fs_open_path(fs_directory_entry_t * root, const char * path) {
    fs_directory_entry_t * cur_node = root;

    fs_directory_entry_add_reference(cur_node);

    uint64_t path_pos = 0;
    uint64_t path_start = 0;
    while (true) {
        if (path[path_pos] == '/' || path[path_pos] == '\0') {
            char segment[path_pos - path_start + 1];
            memcpy(segment, &path[path_start], path_pos - path_start);
            segment[path_pos - path_start] = '\0';

            fs_directory_entry_t * new_node;

            if (segment[0] == '\0' || strcmp(segment, ".") == 0) {
                new_node = cur_node;
            }
            else if (strcmp(segment, "..") == 0) {
                if (cur_node->parent != NULL) {
                    new_node = cur_node->parent;
                    fs_directory_entry_release(cur_node);
                }
                else new_node = cur_node;
            }
            else {
                new_node = fs_directory_entry_enter(cur_node, segment);
                fs_directory_entry_release(cur_node);

                if (new_node == NULL) {
                    return NULL;
                }
            }

            if (path[path_pos] == '\0') {
                return new_node;
            }
            else {
                cur_node = new_node;
            }

            path_start = ++path_pos;
        }

        path_pos++;
    }
}

fs_directory_entry_t * fs_make_path(fs_directory_entry_t * root, const char * path, fs_file_type_t type) {
    fs_directory_entry_t * cur_node = root;

    fs_directory_entry_add_reference(cur_node);

    uint64_t path_pos = 0;
    uint64_t path_start = 0;
    while (true) {
        if (path[path_pos] == '/' || path[path_pos] == '\0') {
            char segment[path_pos - path_start + 1];
            memcpy(segment, &path[path_start], path_pos - path_start);
            segment[path_pos - path_start] = '\0';

            if (path[path_pos] == '\0') {
                fs_directory_entry_t * new_dirent = fs_make(cur_node, segment, type);

                fs_directory_entry_release(cur_node);

                return new_dirent;
            }

            fs_directory_entry_t * new_node;

            if (segment[0] == '\0' || strcmp(segment, ".") == 0) {
                new_node = cur_node;
            }
            else if (strcmp(segment, "..") == 0) {
                if (cur_node->parent != NULL) {
                    new_node = cur_node->parent;
                    fs_directory_entry_release(cur_node);
                }
                else new_node = cur_node;
            }
            else {
                new_node = fs_directory_entry_enter(cur_node, segment);
                fs_directory_entry_release(cur_node);

                if (new_node == NULL) {
                    return NULL;
                }
            }

            cur_node = new_node;

            path_start = ++path_pos;
        }

        path_pos++;
    }
}

fs_directory_entry_t * fs_make_path_dirs(fs_directory_entry_t * root, const char * path, fs_file_type_t type) {
    fs_directory_entry_t * cur_node = root;

    fs_directory_entry_add_reference(cur_node);

    uint64_t path_pos = 0;
    uint64_t path_start = 0;
    while (true) {
        if (path[path_pos] == '/' || path[path_pos] == '\0') {
            char segment[path_pos - path_start + 1];
            memcpy(segment, &path[path_start], path_pos - path_start);
            segment[path_pos - path_start] = '\0';

            if (path[path_pos] == '\0') {
                fs_directory_entry_t * new_dirent = fs_make(cur_node, segment, type);

                fs_directory_entry_release(cur_node);

                return new_dirent;
            }

            fs_directory_entry_t * new_node;

            if (segment[0] == '\0' || strcmp(segment, ".") == 0) {
                new_node = cur_node;
            }
            else if (strcmp(segment, "..") == 0) {
                if (cur_node->parent != NULL) {
                    new_node = cur_node->parent;
                    fs_directory_entry_release(cur_node);
                }
                else new_node = cur_node;
            }
            else {
                new_node = fs_directory_entry_enter(cur_node, segment);
                fs_directory_entry_release(cur_node);

                if (new_node == NULL) {
                    new_node = fs_make(cur_node, segment, FS_DIRECTORY);

                    if (new_node == NULL) return NULL;
                }
            }

            cur_node = new_node;

            path_start = ++path_pos;
        }

        path_pos++;
    }
}


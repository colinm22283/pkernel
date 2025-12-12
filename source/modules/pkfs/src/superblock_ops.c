#include <stddef.h>

#include <util/heap/heap.h>
#include <util/string/strcmp.h>

#include <pkfs.h>
#include <disc_operations.h>

#include <debug/vga_print.h>

fs_node_t * alloc_node(fs_superblock_t * superblock) {
    pkfs_fs_node_t * new_node = heap_alloc_debug(sizeof(pkfs_fs_node_t), "pkfs fsnode");

    fs_node_init(&new_node->base);

    return (fs_node_t *) new_node;
}

error_number_t free_node(fs_superblock_t * superblock, fs_node_t * node) {
    heap_free(node);

    return ERROR_OK;
}

error_number_t list(fs_directory_entry_t * dirent) {
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) dirent->node;

    directory_iterator_t itr;
    if (!directory_iterator_init(dirent->superblock->device, &itr, node->file_page)) return ERROR_UNKNOWN;

    while (true) {
        filesystem_page_address_t page_address;

        filesystem_directory_entry_type_t result = directory_iterator_next(dirent->superblock->device, &itr, &page_address);

        if (page_address == 0) return ERROR_OK;

        char buffer[FILESYSTEM_NAME_MAX_SIZE];

        switch (result) {
            case FS_DET_FILE: {
                get_file_name(dirent->superblock->device, page_address, buffer);
            } break;

            case FS_DET_DIRECTORY: {
                get_directory_name(dirent->superblock->device, page_address, buffer);
            } break;

            case FS_DET_NONE: return ERROR_OK;
        }

        fs_directory_entry_add_entry(dirent, buffer);
    }
}

error_number_t lookup(fs_directory_entry_t * dirent, fs_directory_entry_node_t * dirent_node, fs_node_t * _node) {
    pkfs_fs_node_t * parent_node = (pkfs_fs_node_t *) dirent->node;
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) _node;

    directory_iterator_t itr;
    if (!directory_iterator_init(dirent->superblock->device, &itr, parent_node->file_page)) return ERROR_UNKNOWN;

    while (true) {
        filesystem_page_address_t page_address;

        filesystem_directory_entry_type_t result = directory_iterator_next(dirent->superblock->device, &itr, &page_address);

        if (page_address == 0) return ERROR_FS_NO_ENT;

        char buffer[FILESYSTEM_NAME_MAX_SIZE];
        fs_file_type_t node_type;

        switch (result) {
            case FS_DET_FILE: {
                get_file_name(dirent->superblock->device, page_address, buffer);
                node_type = FS_REGULAR;
            } break;

            case FS_DET_DIRECTORY: {
                get_directory_name(dirent->superblock->device, page_address, buffer);
                node_type = FS_DIRECTORY;
            } break;

            default: continue;
        }

        if (strcmp(buffer, dirent_node->name) == 0) {
            node->file_page = page_address;

            dirent_node->dirent = fs_directory_entry_create(node_type, dirent, dirent_node);
            dirent_node->dirent->node = _node;

            return ERROR_OK;
        }
    }
}

fs_directory_entry_node_t * create(struct fs_directory_entry_s * parent, struct fs_node_s * _node, const char * name, fs_file_type_t type) {
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) _node;
    pkfs_fs_node_t * parent_node = (pkfs_fs_node_t *) parent->node;

    switch (type) {
        case FS_REGULAR: {
            node->file_page = create_file(parent->superblock->device);

            fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(parent, name);

            fs_directory_entry_t * new_dirent = fs_directory_entry_create(type, parent, dirent_node);
            fs_directory_entry_add_reference(new_dirent);

            new_dirent->node = _node;

            dirent_node->dirent = new_dirent;

            link_node(parent->superblock->device, parent_node->file_page, node->file_page, name);

            return dirent_node;
        } break;

        case FS_DIRECTORY: {
            node->file_page = create_dir(parent->superblock->device);

            fs_directory_entry_node_t * dirent_node = fs_directory_entry_add_entry(parent, name);

            fs_directory_entry_t * new_dirent = fs_directory_entry_create(type, parent, dirent_node);
            fs_directory_entry_add_reference(new_dirent);

            new_dirent->node = _node;

            dirent_node->dirent = new_dirent;

            link_node(parent->superblock->device, parent_node->file_page, node->file_page, name);

            return dirent_node;
        } break;

        default: return NULL;
    }
}

error_number_t delete(fs_directory_entry_t * dirent) {
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) dirent->node;

    if (dirent->type == FS_DIRECTORY) {
        return ERROR_UNIMPLEMENTED;
    }
    else {
        if (delete_file(dirent->superblock->device, 64, node->file_page)) return ERROR_OK;
        else return ERROR_UNKNOWN;
    }
}

fs_directory_entry_node_t * link(fs_directory_entry_t * dirent, fs_directory_entry_t * subdirent, const char * name) {
    return NULL;
}

error_number_t write(fs_directory_entry_t * dirent, const char * buffer, uint64_t size, uint64_t offset, uint64_t * wrote) {
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) dirent->node;

    *wrote = write_file(dirent->superblock->device, node->file_page, buffer, size, offset);

    if (*wrote == 0) return ERROR_UNKNOWN;

    return ERROR_OK;
}

error_number_t read(fs_directory_entry_t * dirent, char * buffer, uint64_t size, uint64_t offset, uint64_t * read) {
    pkfs_fs_node_t * node = (pkfs_fs_node_t *) dirent->node;

    *read = read_file(dirent->superblock->device, node->file_page, buffer, size, offset);

    return ERROR_OK;
}

fs_superblock_ops_t superblock_ops = {
    .alloc_node = alloc_node,
    .free_node   = free_node,

    .list = list,
    .lookup = lookup,

    .create = create,
    .delete = delete,

    .link = link,

    .write = write,
    .read = read,
};

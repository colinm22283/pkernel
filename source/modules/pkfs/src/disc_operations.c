#include <stddef.h>

#include <device/device.h>

#include <disc_operations.h>

#include <util/string/strcpy.h>
#include <util/string/strcmp.h>
#include <util/string/strcmpn.h>
#include <util/memory/memcpy.h>
#include <util/math/max.h>

#include <sys/debug/print.h>

static inline bool disc_write(device_t * device, uint64_t lba, uint64_t sectors, const void * buffer) {
    return device->block_ops.write(device, buffer, sectors, lba) == sectors;
}

static inline bool disc_read(device_t * device, uint64_t lba, uint64_t sectors, void * buffer) {
    return device->block_ops.read(device, buffer, sectors, lba) == sectors;
}

static inline uint64_t alloc_page(device_t * device) {
    static filesystem_root_page_t root_node;
    if (!disc_read(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    filesystem_page_address_t alloc_node = root_node.first_free;
    filesystem_page_address_t node = root_node.first_free;

    while (true) {
        node++;

        static filesystem_node_page_t page;
        if (!disc_read(device, node, 1, &page)) return 0;

        if (!page.tag.in_use) break;
    }

    root_node.first_free = node;
    if (!disc_write(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    return alloc_node;
}

pkfs_directory_t open_filesystem(device_t * device, filesystem_page_address_t root_address) {
    filesystem_root_page_t root_page;
    if (!disc_read(device, root_address, 1, &root_page)) return 0;

    if (strcmpn(root_page.signature, FILESYSTEM_ROOT_SIGNATURE, 4) != 0) {
        debug_print("Oh dear... ");
        debug_print_hex(root_page.signature[0]);
        debug_print("\n");

        asm volatile ("hlt");

        return 0;
    }

    return root_page.root_directory_address;
}

pkfs_directory_t open_directory(device_t * device, pkfs_directory_t parent, const char * directory_name) {
    filesystem_directory_node_page_t directory_page;
    if (!disc_read(device, parent, 1, &directory_page)) return 0;

    filesystem_page_address_t directory_index_address = directory_page.directory_index_address;

    while (true) {
        filesystem_directory_index_page_t directory_index;
        if (!disc_read(device, directory_index_address, 1, &directory_index)) return 0;

        for (size_t i = 0; i < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; i++) {
            if (directory_index.children[i] == 0) {
                return 0;
            }
            else {
                filesystem_directory_node_page_t directory_node;
                if (!disc_read(device, directory_index.children[i], 1, &directory_node)) {
                    return 0;
                }

                if (strcmp(directory_node.name, directory_name) == 0) {
                    return directory_index.children[i];
                }
            }
        }

        if (directory_index.next_index_address != 0) {
            directory_index_address = directory_index.next_index_address;
        }
        else return 0;
    }
}

pkfs_file_t open_file(device_t * device, pkfs_directory_t parent, const char * directory_name) {
    filesystem_directory_node_page_t directory_page;
    if (!disc_read(device, parent, 1, &directory_page)) {
        return 0;
    }

    filesystem_page_address_t directory_index_address = directory_page.directory_index_address;

    while (true) {
        filesystem_directory_index_page_t directory_index;
        if (!disc_read(device, directory_index_address, 1, &directory_index)) {
            return 0;
        }

        for (size_t i = 0; i < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; i++) {
            if (directory_index.children[i] == 0) {
                return 0;
            }
            else {
                static filesystem_file_node_page_t file_node;
                if (!disc_read(device, directory_index.children[i], 1, &file_node)) {
                    return 0;
                }

                if (strcmp(file_node.name, directory_name) == 0) {
                    return directory_index.children[i];
                }
            }
        }

        if (directory_index.next_index_address != 0) {
            directory_index_address = directory_index.next_index_address;
        }
        else return 0;
    }
}

bool get_directory_name(device_t * device, pkfs_directory_t file, char * buffer) {
    static filesystem_directory_node_page_t directory_node;

    if (!disc_read(device, file, 1, &directory_node)) {
        return false;
    }
    uint16_t i;
    for (i = 0; i < FILESYSTEM_NAME_MAX_SIZE && directory_node.name[i] != '\0'; i++) buffer[i] = directory_node.name[i];
    buffer[i] = '\0';

    return true;
}

bool get_file_name(device_t * device, pkfs_file_t file, char * buffer) {
    static filesystem_file_node_page_t file_node;

    if (!disc_read(device, file, 1, &file_node)) {
        return false;
    }
    uint16_t i;
    for (i = 0; i < FILESYSTEM_NAME_MAX_SIZE && file_node.name[i] != '\0'; i++) buffer[i] = file_node.name[i];
    buffer[i] = '\0';

    return true;
}

uint64_t get_file_size(device_t * device, pkfs_file_t file) {
    static filesystem_file_node_page_t root_node;

    if (!disc_read(device, file, 1, &root_node)) return 0;

    pkfs_file_t current_file = root_node.root_data_address;
    static filesystem_file_data_page_t file_node;

    uint64_t size = 0;

    while (true) {
        if (!disc_read(device, current_file, 1, &file_node)) return 0;

        size += file_node.size;

        if (file_node.next_data_address != 0) break;

        current_file = file_node.next_data_address;
    }

    return size;
}

pkfs_file_t create_file(device_t * device) {
    static filesystem_root_page_t root_node;
    if (!disc_read(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    filesystem_page_address_t node = root_node.first_free;

    pkfs_file_t file_root_addr = node;
    static filesystem_file_node_page_t file_page;

    file_page.tag.in_use = true;
    file_page.type = FILESYSTEM_PAGE_TYPE_FILE;

    while (true) {
        node++;

        static filesystem_node_page_t page;
        if (!disc_read(device, node, 1, &page)) return 0;

        if (!page.tag.in_use) break;
    }

    static filesystem_file_data_page_t file_data;

    file_data.tag.in_use = true;
    file_data.type = FILESYSTEM_PAGE_TYPE_FILE;
    file_data.parent_file_address = file_root_addr;
    file_data.prev_data_address = 0;
    file_data.next_data_address = 0;
    file_data.size = 0;

    file_page.root_data_address = node;

    if (!disc_write(device, file_root_addr, 1, &file_page)) return 0;
    if (!disc_write(device, node, 1, &file_data)) return 0;

    while (true) {
        node++;

        static filesystem_node_page_t page;
        if (!disc_read(device, node, 1, &page)) return 0;

        if (!page.tag.in_use) break;
    }

    root_node.first_free = node;
    if (!disc_write(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    return file_root_addr;
}

pkfs_file_t create_dir(device_t * device) {
    static filesystem_root_page_t root_node;
    if (!disc_read(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    filesystem_page_address_t node = root_node.first_free;

    pkfs_file_t file_root_addr = node;
    static filesystem_directory_node_page_t dir_page;

    dir_page.tag.in_use = true;
    dir_page.type = FILESYSTEM_PAGE_TYPE_DIRECTORY;

    while (true) {
        node++;

        static filesystem_node_page_t page;
        if (!disc_read(device, node, 1, &page)) return 0;

        if (!page.tag.in_use) break;
    }

    static filesystem_directory_index_page_t file_data;

    file_data.tag.in_use = true;
    file_data.type = FILESYSTEM_PAGE_TYPE_DIRECTORY_INDEX;
    file_data.parent_directory_address = file_root_addr;
    file_data.prev_index_address = 0;
    file_data.next_index_address = 0;
    for (uint64_t i = 0; i < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; i++) file_data.children[i] = 0;

    dir_page.directory_index_address = node;

    if (!disc_write(device, file_root_addr, 1, &dir_page)) return 0;
    if (!disc_write(device, node, 1, &file_data)) return 0;

    while (true) {
        node++;

        static filesystem_node_page_t page;
        if (!disc_read(device, node, 1, &page)) return 0;

        if (!page.tag.in_use) break;
    }

    root_node.first_free = node;
    if (!disc_write(device, FILESYSTEM_ROOT_ADDRESS, 1, &root_node)) return 0;

    return file_root_addr;
}

bool link_node(device_t * device, pkfs_directory_t directory, filesystem_page_address_t node, const char * name) {
    static filesystem_directory_node_page_t node_page;
    if (!disc_read(device, directory, 1, &node_page)) return false;

    static filesystem_directory_index_page_t index_page;
    if (!disc_read(device, node_page.directory_index_address, 1, &index_page)) return false;

    for (size_t i = 0; i < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; i++) {
        if (index_page.children[i] == 0) {
            index_page.children[i] = node;

            static filesystem_file_node_page_t file_page;
            if (!disc_read(device, node, 1, &file_page)) return false;

            strcpy(file_page.name, name);
            file_page.parent_directory_address = directory;

            if (!disc_write(device, node, 1, &file_page)) return false;

            if (!disc_write(device, node_page.directory_index_address, 1, &index_page)) return false;

            return true;
        }
    }

    return false;
}

bool directory_iterator_init(device_t * device, directory_iterator_t * iterator, pkfs_directory_t directory) {
    if (!disc_read(device, directory, 1, &iterator->node_page)) {
        return false;
    }
    if (!disc_read(device, iterator->node_page.directory_index_address, 1, &iterator->index_page)) {
        return false;
    }

    iterator->index_location = 0;

    return true;
}

filesystem_directory_entry_type_t directory_iterator_next(device_t * device, directory_iterator_t * iterator, filesystem_page_address_t * handle) {
    if (iterator->index_location >= FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE) {
        if (!disc_read(device, iterator->index_page.next_index_address, 1, &iterator->index_page)) {
            return FS_DET_NONE;
        }

        iterator->index_location = 0;

        return directory_iterator_next(device, iterator, handle);
    }
    else if (iterator->index_page.children[iterator->index_location] == 0) {
        return FS_DET_NONE;
    }
    else {
        *handle = iterator->index_page.children[iterator->index_location];
        iterator->index_location++;

        filesystem_node_page_t page;

        if (!disc_read(device, *handle, 1, &page)) {
            return FS_DET_NONE;
        }

        switch (page.type) {
            case FILESYSTEM_PAGE_TYPE_FILE: return FS_DET_FILE;
            case FILESYSTEM_PAGE_TYPE_DIRECTORY: return FS_DET_DIRECTORY;
            default: return FS_DET_NONE;
        }
    }
}

uint64_t read_file(device_t * device, pkfs_file_t file, char * buffer, uint64_t size, uint64_t offset) {
    static filesystem_file_node_page_t file_node;
    if (!disc_read(device, file, 1, &file_node)) return 0;

    static filesystem_file_data_page_t data_page;
    if (!disc_read(device, file_node.root_data_address, 1, &data_page)) return 0;

    uint64_t inner_offset = 0;

    while (offset > 0) {
        if (inner_offset < data_page.size) {
            inner_offset++;
            offset--;
        }
        else {
            if (data_page.next_data_address == 0) return 0;

            if (!disc_read(device, data_page.next_data_address, 1, &data_page)) return 0;

            inner_offset = 0;
        }
    }

    uint64_t buffer_offset = 0;
    while (size > 0) {
        if (inner_offset < data_page.size) {
            buffer[buffer_offset] = data_page.data[inner_offset];

            buffer_offset++;
            inner_offset++;
            size--;
        }
        else {
            if (data_page.next_data_address == 0) return buffer_offset;

            if (!disc_read(device, data_page.next_data_address, 1, &data_page)) return buffer_offset;

            inner_offset = 0;
        }
    }

    return buffer_offset;
}

uint64_t write_file(device_t * device, pkfs_file_t file, const char * buffer, uint64_t size, uint64_t offset) {
    static filesystem_file_node_page_t file_node;
    if (!disc_read(device, file, 1, &file_node)) return 0;

    debug_print("WRITE FILE\n");
    debug_print(file_node.name);
    debug_print("\n");
    debug_print_hex(offset);
    debug_print(", ");
    debug_print_hex(size);
    debug_print("\n");

    uint64_t pos = 0;
    uint64_t buffer_off = 0;
    filesystem_page_address_t data_addr = file_node.root_data_address;
    static filesystem_file_data_page_t file_data;
    while (true) {
        if (!disc_read(device, data_addr, 1, &file_data)) return 0;

        if (offset - pos < FILESYSTEM_FILE_DATA_PAGE_SIZE) {
            debug_print("WRITING\n");

            const uint64_t write_size = MIN(size, FILESYSTEM_FILE_DATA_PAGE_SIZE - file_data.size);

            memcpy(file_data.data + file_data.size, buffer + buffer_off, write_size);

            buffer_off += write_size;
            file_data.size += write_size;
            pos += file_data.size;

            if (!disc_write(device, data_addr, 1, &file_data)) return 0;

            break;
        }

        if (file_data.next_data_address == 0) {
            debug_print("NEW PAGE\n");

            static filesystem_file_data_page_t new_page = {
                .type = FILESYSTEM_PAGE_TYPE_FILE_DATA,
                .tag = {
                    .in_use = true,
                },
                .size = 0,
                .next_data_address = 0,
            };

            new_page.prev_data_address = data_addr;

            file_data.next_data_address = alloc_page(device);

            if (!disc_write(device, file_data.next_data_address, 1, &new_page)) return 0;
        }

        data_addr = file_data.next_data_address;
    }

    return size;
}

bool delete_file(device_t * device, filesystem_page_address_t root_address, pkfs_file_t file) {
    debug_print("REMOVE\n");
    filesystem_root_page_t root_page;
    if (!disc_read(device, root_address, 1, &root_page)) return 0;

    static filesystem_file_node_page_t file_node;
    if (!disc_read(device, file, 1, &file_node)) return 0;

    filesystem_page_address_t addr = file_node.root_data_address;

    while (addr != 0) {
        static filesystem_file_data_page_t data_node;
        if (!disc_read(device, addr, 1, &data_node)) return 0;

        filesystem_page_address_t next = data_node.next_data_address;

        if (addr < root_page.first_free) root_page.first_free = addr;
        data_node.tag.in_use = false;

        if (!disc_write(device, addr, 1, &data_node)) return 0;

        addr = next;
    }

    if (file < root_page.first_free) root_page.first_free = file;
    file_node.tag.in_use = false;

    pkfs_directory_t dir = file_node.parent_directory_address;

    static filesystem_directory_node_page_t dir_node;
    if (!disc_read(device, dir, 1, &dir_node)) return 0;

    filesystem_page_address_t index_addr = dir_node.directory_index_address;
    while (index_addr != 0) {
        static filesystem_directory_index_page_t index_node;
        if (!disc_read(device, index_addr, 1, &index_node)) return 0;

        bool found = false;

        for (uint64_t i = 0; i < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; i++) {
            if (index_node.children[i] == file) {
                for (uint64_t j = i; j < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE - 1; j++) {
                    index_node.children[j] = index_node.children[j + 1];
                }
                index_node.children[FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE - 1] = 0;

                found = true;
                break;
            }
        }

        if (found) {
            if (!disc_write(device, index_addr, 1, &index_node)) return 0;

            break;
        }
        else {
            index_addr = index_node.next_index_address;
        }
    }

    if (!disc_write(device, root_address, 1, &root_page)) return 0;

    return true;
}

#include <device/device.h>

#include <util/memory/memcpy.h>
#include <util/string/strcmpn.h>
#include <util/string/strcmp.h>

#include <disc_operations.h>

static inline bool disc_write(device_t * device, uint64_t lba, uint64_t sectors, const void * buffer) {
    return device->block_ops.write(device, buffer, lba * 512, sectors * 512) == sectors * 512;
}

static inline bool disc_read(device_t * device, uint64_t lba, uint64_t sectors, void * buffer) {
    return device->block_ops.read(device, buffer, lba * 512, sectors * 512) == sectors * 512;
}

static inline filesystem_page_address_t advance_root_first_free(device_t * device, filesystem_page_address_t first_free) {
    while (true) {
        first_free++;

        filesystem_node_page_t file_node;

        disc_read(device, first_free, 1, (void *) &file_node);

        if (!file_node.tag.in_use) return first_free;
    }
}

directory_t open_filesystem(device_t * device, filesystem_page_address_t root_address) {
    filesystem_root_page_t root_page;
    if (!disc_read(device, root_address, 1, &root_page)) return 0;

    if (strcmpn(root_page.signature, FILESYSTEM_ROOT_SIGNATURE, 4) != 0) {
        return 0;
    }

    return root_page.root_directory_address;
}

directory_t open_directory(device_t * device, directory_t parent, const char * directory_name) {
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
pkfs_file_t open_file(device_t * device, directory_t parent, const char * directory_name) {
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
                filesystem_file_node_page_t file_node;
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

bool get_directory_name(directory_t file, char * buffer) {
    filesystem_directory_node_page_t directory_node;

    if (!disc_read(device, file, 1, &directory_node)) {
        return false;
    }
    uint16_t i;
    for (i = 0; i < FILESYSTEM_NAME_MAX_SIZE && directory_node.name[i] != '\0'; i++) buffer[i] = directory_node.name[i];
    buffer[i] = '\0';

    return true;
}
bool get_file_name(file_t file, char * buffer) {
    filesystem_file_node_page_t file_node;

    if (!disc_read(device, file, 1, &file_node)) {
        return false;
    }
    uint16_t i;
    for (i = 0; i < FILESYSTEM_NAME_MAX_SIZE && file_node.name[i] != '\0'; i++) buffer[i] = file_node.name[i];
    buffer[i] = '\0';

    return true;
}

bool directory_iterator_init(directory_iterator_t * iterator, directory_t directory) {
    if (!disc_read(device, directory, 1, &iterator->node_page)) {
        return false;
    }
    if (!disc_read(device, iterator->node_page.directory_index_address, 1, &iterator->index_page)) {
        return false;
    }

    iterator->index_location = 0;

    return true;
}

filesystem_directory_entry_type_t directory_iterator_next(directory_iterator_t * iterator, filesystem_page_address_t * handle) {
    if (iterator->index_location >= FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE) {
        if (!disc_read(device, iterator->index_page.next_index_address, 1, &iterator->index_page)) {
            return FS_DET_NONE;
        }

        iterator->index_location = 0;

        return directory_iterator_next(iterator, handle);
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

bool file_reader_init(file_reader_t * reader, file_t file) {
    filesystem_file_node_page_t file_node;
    if (!disc_read(device, file, 1, &file_node)) {
        return false;
    }

    if (!disc_read(device, file_node.root_data_address, 1, &reader->data_page)) {
        return false;
    }

    reader->data_address = file_node.root_data_address;
    reader->data_location = 0;

    return true;
}
uint32_t file_reader_read(file_reader_t * reader, char * buffer, uint32_t bytes) {
    if (reader->data_address == 0) return 0;

    uint32_t read_bytes = 0;

    while (reader->data_address != 0 && bytes > 0) {
        if (reader->data_location + bytes >= reader->data_page.size) {
            memcpy(&buffer[read_bytes], &reader->data_page.data[reader->data_location], reader->data_page.size - reader->data_location);
            read_bytes += reader->data_page.size - reader->data_location;
            bytes -= reader->data_page.size - reader->data_location;
            reader->data_location = 0;

            reader->data_address = reader->data_page.next_data_address;
            if (!disc_read(device, reader->data_address, 1, &reader->data_page)) {
                return false;
            }
        }
        else {
            memcpy(&buffer[read_bytes], &reader->data_page.data[reader->data_location], bytes);
            read_bytes += bytes;
            reader->data_location += bytes;
            bytes = 0;
        }
    }

    return read_bytes;
}

directory_t open_directory_path(directory_t root, const char * path) {
    directory_t directory = root;

    uint32_t base_addr = 0;
    uint32_t i;
    for (i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            char buffer[FILESYSTEM_NAME_MAX_SIZE];
            memcpy(buffer, &path[base_addr], i - base_addr);
            buffer[i - base_addr] = '\0';

            directory = open_directory(directory, buffer);
            if (directory == 0) return 0;

            base_addr = i + 1;
        }
    }

    char buffer[FILESYSTEM_NAME_MAX_SIZE];
    memcpy(buffer, &path[base_addr], i - base_addr + 1);

    directory_t dir = open_directory(directory, buffer);

    return dir;
}

file_t open_file_path(directory_t root, const char * path) {
    directory_t directory = root;

    uint32_t base_addr = 0;
    uint32_t i;
    for (i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            char buffer[FILESYSTEM_NAME_MAX_SIZE];
            memcpy(buffer, &path[base_addr], i - base_addr);
            buffer[i - base_addr] = '\0';

            directory = open_directory(directory, buffer);
            if (directory == 0) return 0;

            base_addr = i + 1;
        }
    }

    char buffer[FILESYSTEM_NAME_MAX_SIZE];
    memcpy(buffer, &path[base_addr], i - base_addr + 1);

    file_t file = open_file(directory, buffer);

    return file;
}

bool stat_file(file_stat_result_t * result, filesystem_page_address_t address) {
    filesystem_node_page_t _node_page;
    if (!disc_read(device, address, 1, &_node_page)) return false;

    if (_node_page.type == FILESYSTEM_PAGE_TYPE_FILE) {
        filesystem_file_node_page_t * node_page = (filesystem_file_node_page_t *) &_node_page;

        uint64_t size = 0;
        uint32_t sectors = 0;

        filesystem_page_address_t data_address = node_page->root_data_address;
        filesystem_file_data_page_t data_page;

        while (data_address != 0) {
            if (!disc_read(device, data_address, 1, &data_page)) return false;

            size += data_page.size;
            sectors++;

            data_address = data_page.next_data_address;
        }

        result->type = FILESYSTEM_PAGE_TYPE_FILE;
        result->size = size;
        result->size_on_disc = (uint64_t) sectors * 512 + 512;
    }
    else if (_node_page.type == FILESYSTEM_PAGE_TYPE_DIRECTORY) {
        filesystem_directory_node_page_t * node_page = (filesystem_directory_node_page_t *) &_node_page;

        uint32_t sectors = 0;

        filesystem_page_address_t data_address = node_page->directory_index_address;
        filesystem_directory_index_page_t index_page;

        while (data_address != 0) {
            if (disc_read(device, data_address, 1, &index_page)) return false;

            sectors++;

            data_address = index_page.next_index_address;
        }

        result->type = FILESYSTEM_PAGE_TYPE_DIRECTORY;
        result->size = result->size_on_disc = (uint64_t) sectors * 512 + 512;
    }
    else return false;

    return true;
}

bool append_file(filesystem_page_address_t root_address, file_t file, const char * content, uint32_t size) {
    filesystem_root_page_t root_page;
    disc_read(device, root_address, 1, (uint16_t *) &root_page);

    filesystem_file_node_page_t file_node_page;
    disc_read(device, file, 1, (void *) &file_node_page);

    filesystem_page_address_t file_data_address = file_node_page.root_data_address;
    filesystem_file_data_page_t file_data_page;

    while (true) {
        disc_read(device, file_data_address, 1, (void *) &file_data_page);

        uint16_t available_size = FILESYSTEM_FILE_DATA_PAGE_SIZE - file_data_page.size;

        if (available_size == 0) {
            file_data_address = file_data_page.next_data_address;
        }
        else break;
    }

    uint32_t current_pos = 0;

    while (current_pos < size) {
        uint16_t remaining_size = size - current_pos;
        uint16_t available_size = FILESYSTEM_FILE_DATA_PAGE_SIZE - file_data_page.size;

        if (available_size > remaining_size) {
            memcpy(file_data_page.data + file_data_page.size, &content[current_pos], remaining_size);
            file_data_page.size += remaining_size;
            disc_write(device, file_data_address, 1, (uint16_t *) &file_data_page);

            current_pos += remaining_size;
        }
        else {
            memcpy(file_data_page.data + file_data_page.size, &content[current_pos], available_size);
            file_data_page.next_data_address = root_page.first_free;
            file_data_page.size += available_size;
            disc_write(device, file_data_address, 1, (uint16_t *) &file_data_page);

            current_pos += available_size;

            filesystem_file_data_page_t new_file_data_page = {
                .tag.in_use = 1,
                .type = FILESYSTEM_PAGE_TYPE_FILE_DATA,
                .parent_file_address = file,
                .prev_data_address = file_data_address,
                .next_data_address = 0,
                .size = 0,
            };
            disc_write(device, root_page.first_free, 1, (uint16_t *) &new_file_data_page);
            root_page.first_free = advance_root_first_free(root_page.first_free);

            file_data_address = file_data_page.next_data_address;
            disc_read(device, file_data_address, 1, (void *) &file_data_page);
        }
    }

    disc_write(device, root_address, 1, (uint16_t *) &root_page);

    return true;
}

bool create_file(filesystem_page_address_t root_page_address, directory_t parent_directory, const char * name) {
    filesystem_root_page_t root_page;
    disc_read(device, root_page_address, 1, (uint16_t *) &root_page);

    filesystem_directory_node_page_t directory_node_page;
    disc_read(device, parent_directory, 1, (uint16_t *) &directory_node_page);

    filesystem_page_address_t directory_index_address = directory_node_page.directory_index_address;
    filesystem_directory_index_page_t directory_index_page;

    while (true) {
        disc_read(device, directory_index_address, 1, (uint16_t *) &directory_index_page);

        for (uint32_t directory_child_index = 0; directory_child_index < FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE; directory_child_index++) {
            if (directory_index_page.children[directory_child_index] == 0) {
                directory_index_page.children[directory_child_index] = root_page.first_free;

                filesystem_page_address_t node_page_address = root_page.first_free;
                root_page.first_free = advance_root_first_free(root_page.first_free);
                filesystem_page_address_t data_page_address = root_page.first_free;
                root_page.first_free = advance_root_first_free(root_page.first_free);

                filesystem_file_node_page_t file_node_page = {
                    .tag.in_use = 1,
                    .type = FILESYSTEM_PAGE_TYPE_FILE,
                    .parent_directory_address = parent_directory,
                    .root_data_address = data_page_address,
                };
                strcpy(file_node_page.name, name);

                filesystem_file_data_page_t file_data_page = {
                    .tag.in_use = 1,
                    .type = FILESYSTEM_PAGE_TYPE_FILE_DATA,
                    .size = 0,
                    .next_data_address = 0,
                    .prev_data_address = 0,
                    .parent_file_address = node_page_address,
                };

                if (disc_write(device, node_page_address, 1, (uint16_t *) &file_node_page) != 0) {
                    return false;
                }

                if (disc_write(device, data_page_address, 1, (uint16_t *) &file_data_page) != 0) {
                    return false;
                }

                if (directory_child_index == FILESYSTEM_DIRECTORY_INDEX_CHILDREN_SIZE - 1) {
                    filesystem_directory_index_page_t new_directory_index = {
                        .tag.in_use = 1,
                        .type = FILESYSTEM_PAGE_TYPE_DIRECTORY_INDEX,
                        .parent_directory_address = parent_directory,
                        .prev_index_address = directory_index_address,
                        .next_index_address = 0,
                    };
                    directory_index_page.next_index_address = root_page.first_free;
                    disc_write(device, root_page.first_free, 1, (uint16_t *) &new_directory_index);

                    root_page.first_free = advance_root_first_free(root_page.first_free);
                }

                disc_write(device, directory_index_address, 1, (uint16_t *) &directory_index_page);

                disc_write(device, root_page_address, 1, (uint16_t *) &root_page);

                return true;
            }
        }

        if (directory_index_page.next_index_address == 0) return false;

        directory_index_address = directory_index_page.next_index_address;
    }
}
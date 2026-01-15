#include <filesystem/file.h>
#include <sys/debug/print.h>

error_number_t file_init(fs_file_t * file, fs_directory_entry_t * dirent, open_options_t options) {
    if (dirent->type == FS_DIRECTORY && (options & OPEN_WRITE)) return ERROR_IS_DIR;

    file->dirent = dirent;
    file->options = options;
    file->offset = 0;

    if (dirent->type == FS_DIRECTORY) {
        file->current_node = file->dirent->head.next;
    }

    return ERROR_OK;
}

error_number_t file_clone(fs_file_t * dst, fs_file_t * src) {
    dst->dirent = src->dirent;

    dst->options = src->options;
    dst->offset = src->offset;

    fs_directory_entry_add_reference(dst->dirent);

    if (dst->dirent->type == FS_DIRECTORY) {
        dst->current_node = dst->dirent->head.next;
    }

    return ERROR_OK;
}

int64_t file_read(fs_file_t * file, char * buffer, uint64_t size) {
    if (!(file->options & OPEN_READ)) return ERROR_BAD_PERM;

    if (file->dirent->type == FS_DIRECTORY) return ERROR_IS_DIR;

    uint64_t amount_read;

    if (file->dirent->type == FS_PIPE) {
        pipe_read(file->dirent, buffer, size, 0, &amount_read);

        return (int64_t) amount_read;
    }

    if (file->dirent->type == FS_SOCKET) {
        socket_read(file->dirent->socket, buffer, size, &amount_read);

        return (int64_t) amount_read;
    }

    error_number_t result = file->dirent->superblock->superblock_ops->read(file->dirent, buffer, size, file->offset, &amount_read);
    file->offset += amount_read;

    if (result != ERROR_OK) return result;

    return (int64_t) amount_read;
}

int64_t file_write(fs_file_t * file, const char * buffer, uint64_t size) {
    if (!(file->options & OPEN_WRITE)) return ERROR_BAD_PERM;

    switch (file->dirent->type) {
        case FS_REGULAR:
        case FS_DEVICE: {
            uint64_t amount_written;

            error_number_t result = file->dirent->superblock->superblock_ops->write(file->dirent, buffer, size, file->offset, &amount_written);
            file->offset += amount_written;

            if (result != ERROR_OK) return result;

            return (int64_t) amount_written;
        } break;

        case FS_DIRECTORY: {
            return ERROR_IS_DIR;
        } break;

        case FS_PIPE: {
            uint64_t amount_written;

            error_number_t result = pipe_write(file->dirent, buffer, size, 0, &amount_written);

            if (result != ERROR_OK) return result;

            return (int64_t) amount_written;
        } break;

        case FS_SOCKET: {
            uint64_t amount_written;

            error_number_t result = socket_write(file->dirent->socket, buffer, size, &amount_written);

            if (result != ERROR_OK) return result;

            return (int64_t) amount_written;
        } break;
    }

    return ERROR_UNKNOWN;
}

void * file_map(fs_file_t * file, pman_context_t * context, void * map_addr, uint64_t size, uint64_t offset) {
    switch (file->dirent->type) {
        case FS_DEVICE: {
            device_t * device = file->dirent->device;

            switch (device->type) {
                case DT_CHARACTER: {
                    return device->char_ops.map(device, context, file->options & OPEN_WRITE ? PMAN_PROT_WRITE : 0, map_addr, size, offset);
                } break;

                default: return (void *) ERROR_UNIMPLEMENTED;
            }
        } break;

        default: {
            return (void *) ERROR_IS_DIR;
        } break;
    }
}

void file_close(fs_file_t * file) {
    fs_directory_entry_release(file->dirent);
}

int64_t file_readdir(fs_file_t * file, directory_entry_t * entries, uint64_t buffer_size) {
    if (!(file->options & OPEN_READ)) return ERROR_BAD_PERM;

    if (file->dirent->type != FS_DIRECTORY) return ERROR_NOT_DIR;

    if (file->current_node == &file->dirent->tail) return ERROR_OK;

    uint64_t buffer_pos = 0;
    char * buffer = (char *) entries;

    while (true) {
        uint64_t name_length = strlen(file->current_node->name);
        uint64_t entry_length = sizeof(directory_entry_t) + name_length + 1;

        if (buffer_pos + entry_length > buffer_size) break;

        directory_entry_t * entry = (directory_entry_t *) (buffer + buffer_pos);

        fs_directory_entry_t * dirent = fs_directory_node_enter(file->dirent, file->current_node);

        entry->file_type = dirent->type;
        entry->struct_size = entry_length;

        fs_directory_entry_release(dirent);

        strcpy(entry->name, file->current_node->name);

        buffer_pos += entry_length;

        file->current_node = file->current_node->next;

        if (file->current_node == &file->dirent->tail) break;
    }

    return (int64_t) buffer_pos;
}

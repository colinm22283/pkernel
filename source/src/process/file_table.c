#include <process/file_table.h>

#include <util/heap/heap.h>

void file_table_set(file_table_t * file_table, fd_t fd, fs_file_t * file) {
    if (fd >= file_table->file_capacity) {
        size_t prev_cap = file_table->file_capacity;
        file_table->file_capacity = fd + 1;

        file_table->files = heap_realloc(file_table->files, file_table->file_capacity * sizeof(fs_file_t *));

        for (size_t i = prev_cap; i < file_table->file_capacity; i++) file_table->files[i] = NULL;
    }

    if (file_table->files[fd] != NULL) {
        file_close(file_table->files[fd]);
        heap_free(file_table->files[fd]);
    }

    file_table->files[fd] = file;
}

fd_t file_table_add(file_table_t * file_table, fs_file_t * file) {
    fd_t fd = 0;
    while (fd < file_table->file_capacity) {
        if (file_table->files[fd] == NULL) break;

        fd++;
    }

    if (fd >= file_table->file_capacity) {
        size_t prev_cap = file_table->file_capacity;
        file_table->file_capacity = fd + 1;

        file_table->files = heap_realloc(file_table->files, file_table->file_capacity * sizeof(fs_file_t *));

        for (size_t i = prev_cap; i < file_table->file_capacity; i++) file_table->files[i] = NULL;
    }

    file_table->files[fd] = file;

    return fd;
}

void file_table_init(file_table_t * file_table) {
    file_table->file_capacity = 1;
    file_table->files = heap_alloc(file_table->file_capacity * sizeof(fs_file_t *));

    for (size_t i = 0; i < file_table->file_capacity; i++) file_table->files[i] = NULL;
}

void file_table_free(file_table_t * file_table) {
    for (size_t i = 0; i < file_table->file_capacity; i++) {
        if (file_table->files[i] != NULL) {
            file_close(file_table->files[i]);
            heap_free(file_table->files[i]);
        }
    }

    heap_free(file_table->files);
}

void file_table_clone(file_table_t * dst, file_table_t * src) {
    dst->file_capacity = src->file_capacity;
    dst->files = heap_realloc(dst->files, dst->file_capacity * sizeof(fs_file_t *));

    for (size_t i = 0; i < src->file_capacity; i++) {
        if (src->files[i] != NULL) {
            dst->files[i] = heap_alloc(sizeof(fs_file_t));

            file_clone(dst->files[i], src->files[i]);
        }
        else dst->files[i] = NULL;
    }
}

error_number_t file_table_dup(file_table_t * file_table, fd_t dst, fd_t src) {
    if (src >= file_table->file_capacity || file_table->files[src] == NULL) return ERROR_BAD_FD;

    fs_file_t * file = heap_alloc(sizeof(fs_file_t));

    file_init(file, file_table->files[src]->dirent, file_table->files[src]->options);

    file_table_set(file_table, dst, file);

    return ERROR_OK;
}

fd_t file_table_open(file_table_t * file_table, fs_directory_entry_t * node, open_options_t options) {
    fs_file_t * file = heap_alloc(sizeof(fs_file_t));

    error_number_t init_result = file_init(file, node, options);
    if (init_result != ERROR_OK) return init_result;

    return file_table_add(file_table, file);
}

fs_file_t * file_table_get(file_table_t * file_table, fd_t fd) {
    if (fd >= file_table->file_capacity || file_table->files[fd] == NULL) return NULL;

    return file_table->files[fd];
}

error_number_t file_table_close(file_table_t * file_table, fd_t fd) {
    if (fd >= file_table->file_capacity || file_table->files[fd] == NULL) return ERROR_BAD_FD;

    file_close(file_table->files[fd]);
    heap_free(file_table->files[fd]);

    return ERROR_OK;
}

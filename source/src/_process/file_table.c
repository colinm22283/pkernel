#include <stddef.h>

#include <_process/file_table.h>

#include <util/heap/heap.h>

#include <pkos/defs.h>

static inline error_number_t set_table_entry(process_file_table_t * pft, fs_directory_entry_t * node, fd_t fd, open_options_t options) {
    fs_file_t * new_file = heap_alloc(sizeof(fs_file_t));

    error_number_t init_result = file_init(new_file, node, options);
    if (init_result != ERROR_OK) return init_result;

    if (pft->files[fd] != NULL) {
        file_close(pft->files[fd]);
        heap_free(pft->files[fd]);
    }

    pft->files[fd] = new_file;

    return ERROR_OK;
}

error_number_t process_file_table_init(process_file_table_t * pft) {
    pft->file_count = 2;
    pft->file_capacity = 4;

    pft->files = heap_alloc(pft->file_capacity * sizeof(fs_file_t *));

    pft->files[0] = NULL;
    pft->files[1] = NULL;

    return ERROR_OK;
}

error_number_t process_file_table_clone(process_file_table_t * dst, process_file_table_t * src) {
    dst->file_count    = src->file_count;
    dst->file_capacity = src->file_capacity;

    dst->files = heap_alloc(dst->file_capacity * sizeof(fs_file_t *));

    for (int64_t i = 0; i < src->file_count; i++) {
        if (src->files[i] != NULL) {
            dst->files[i] = heap_alloc(sizeof(fs_file_t));

            file_clone(dst->files[i], src->files[i]);
        }
        else dst->files[i] = NULL;
    }

    return ERROR_OK;
}

void process_file_table_free(process_file_table_t * pft) {
    for (int64_t i = 0; i < pft->file_count; i++) {
        process_file_table_close(pft, i);
    }
    heap_free(pft->files);
}

fd_t process_file_table_set(process_file_table_t * pft, fd_t fd, fs_directory_entry_t * node, open_options_t options) {
    error_number_t set_entry_result = set_table_entry(pft, node, fd, options);
    if (set_entry_result != ERROR_OK) return set_entry_result;
    return fd;
}

fd_t process_file_table_open(process_file_table_t * pft, fs_directory_entry_t * node, open_options_t options) {
    for (int64_t i = 2; i < pft->file_count; i++) {
        if (pft->files[i] == NULL) {
            error_number_t set_entry_result = set_table_entry(pft, node, i, options);
            if (set_entry_result != ERROR_OK) return set_entry_result;

            return i;
        }
    }

    fd_t fd = pft->file_count++;
    pft->files[fd] = NULL;

    error_number_t set_entry_result = set_table_entry(pft, node, fd, options);
    if (set_entry_result != ERROR_OK) return set_entry_result;

    if (pft->file_count == pft->file_capacity) {
        pft->file_capacity *= 2;

        pft->files = heap_realloc(pft->files, pft->file_capacity * sizeof(fs_file_t *));
    }

    return fd;
}

error_number_t process_file_table_close(process_file_table_t * pft, fd_t fd) {
    fs_file_t * file = process_file_table_get(pft, fd);

    if (file == NULL) return ERROR_BAD_FD;

    file_close(file);

    heap_free(file);
    pft->files[fd] = NULL;

    return ERROR_OK;
}

fs_file_t * process_file_table_get(process_file_table_t * pft, fd_t fd) {
    if (fd < 0 || fd >= pft->file_count) return NULL;

    return pft->files[fd];
}

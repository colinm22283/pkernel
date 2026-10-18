#pragma once

#include <pman/types.h>

#include <filesystem/file.h>

pman_source_t * pman_source_init_file(fs_file_t * file, size_t offset);
pman_source_t * pman_source_init_anon(void);
pman_source_t * pman_source_init_anon_kernel(size_t paddr);

void pman_source_add_ref(pman_source_t * source);
void pman_source_sub_ref(pman_source_t * source);

void pman_source_page_in(pman_source_t * source);
void * pman_source_map_to_kernel(pman_source_t * source);


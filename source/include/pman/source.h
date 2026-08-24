#pragma once

#include <pman/types.h>

pman_source_t * pman_source_init_file(file_t * file, size_t offset);
pman_source_t * pman_source_init_anon(void);
void pman_source_deref(pman_source_t * source);


#pragma once

#include <pman/types.h>

static inline bool pman_prot_flags_read_write(pman_mapping_flags_t flags) {
    return !!(flags | PMAN_WRITE);
}

static inline bool pman_prot_flags_execute_disable(pman_mapping_flags_t flags) {
    return !(flags | PMAN_EXECUTE);
}


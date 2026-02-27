#pragma once

#include <sys/paging/load_page_table.h>
#include <sys/paging/read_page_table.h>

static inline void reload_page_table(void) {
    load_page_table((void *) read_page_table());
}

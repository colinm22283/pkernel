#pragma once

#include <stdbool.h>

#include <process/process.h>

typedef struct procfs_node_s {
    process_t * process;

    struct procfs_node_s * next;
    struct procfs_node_s * prev;
} procfs_node_t;

bool fs_procfs_init(void);

procfs_node_t * fs_procfs_register()

#pragma once

#include <process/process.h>

#include <errno.h>

void prog_loader_init(void);
int load_program(process_t * process, fs_directory_entry_t * dirent);
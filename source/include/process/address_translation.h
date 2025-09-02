#pragma once

#include <process/process.h>

void * process_user_to_kernel(process_t * process, const void * user_vaddr);
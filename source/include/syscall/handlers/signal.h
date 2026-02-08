#pragma once

#include <stdint.h>

#include <pkos/types.h>

#include <defs.h>

error_number_t syscall_signal(signal_number_t sig, signal_handler_t * handler);

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <sys/port.h>

void io_arbitrator_init(void);

bool io_arbitrator_test_reserved(port_t port);
bool io_arbitrator_reserve(port_t port);
bool io_arbitrator_release(port_t port);
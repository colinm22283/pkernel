#pragma once

#include <stdint.h>

#include <sys/ata/pio.h>

typedef enum { DEVICE_DRIVE_MASTER, DEVICE_DRIVE_SLAVE } device_drive_t;

void disc_select(port_t io_port, port_t control_port, device_drive_t drive);
bool disc_present(port_t io_port, port_t control_port, device_drive_t drive);

bool disc_read(uint64_t lba, uint64_t sector_count, void * dst);
bool disc_write(uint64_t lba, uint64_t sector_count, const void * src);

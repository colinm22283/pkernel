#include <stdbool.h>

#include <pio_ops.h>

#include <sys/asm/in.h>
#include <sys/asm/out.h>
#include <sys/ata/pio.h>
#include <sys/ports.h>

#include "../../../../../pkbl/source/bootloader/include/boot/disc.h"
#include "debug/vga_print.h"

typedef union {
    uint8_t num;
    ata_pio_status_t bits;
} status_pack_t;

device_drive_t current_device_drive = -1;
port_t current_io_port = -1;
port_t current_control_port = -1;

static inline void pio_write8(port_t port, port_t offset, uint8_t value) {
    outb(port + offset, value);
}
static inline void pio_write16(port_t port, port_t offset, uint16_t value) {
    outw(port + offset, value);
}
static inline uint8_t pio_read8(port_t port, port_t offset) {
    return inb(port + offset);
}
static inline uint16_t pio_read16(port_t port, port_t offset) {
    return inw(port + offset);
}

static inline uint8_t read_status_long(port_t io_port) {
    for (int i = 0; i < 14; i++) pio_read8(io_port, ATA_PIO_IO_STATUS_OFFSET);
    return pio_read8(io_port, ATA_PIO_IO_STATUS_OFFSET);
}

static inline status_pack_t wait_ready(void) {
    volatile status_pack_t status = { .num = pio_read8(current_control_port, ATA_PIO_CONTROL_STATUS_OFFSET) };

    while (
        (status.bits.busy) &&
        (!status.bits.error) &&
        (!status.bits.drive_fault)
    ) {
        status.num = pio_read8(current_control_port, ATA_PIO_CONTROL_STATUS_OFFSET);
    }

    return status;
}

static inline status_pack_t wait_data_ready(void) {
    volatile status_pack_t status = { .num = pio_read8(current_io_port, ATA_PIO_IO_STATUS_OFFSET) };

    while (
        (status.bits.busy || !status.bits.data_ready) &&
        (!status.bits.error) &&
        (!status.bits.drive_fault)
    ) {
        status.num = pio_read8(current_io_port, ATA_PIO_IO_STATUS_OFFSET);
    }

    return status;
}

static inline void disc_reset(void) {
    pio_write8(current_control_port, ATA_PIO_CONTROL_DEVICE_CONTROL_OFFSET, ATA_PIO_DEVICE_CONTROL_SOFTWARE_RESET);
    pio_write8(current_control_port, ATA_PIO_CONTROL_DEVICE_CONTROL_OFFSET, 0);

    for (uint8_t i = 0; i < 4; i++) pio_read8(current_control_port, ATA_PIO_CONTROL_STATUS_OFFSET);
}

static inline void cache_flush(void) {
    pio_write8(current_io_port, ATA_PIO_IO_COMMAND_OFFSET, ATA_PIO_COMMAND_CACHE_FLUSH);

    wait_ready();
}

void disc_select(port_t io_port, port_t control_port, device_drive_t drive) {
    if (io_port != current_io_port || control_port != current_control_port || drive != current_device_drive) {
        if (drive == DEVICE_DRIVE_MASTER) {
            pio_write8(io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xA0);
        }
        else {
            pio_write8(io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xB0);
        }

        read_status_long(io_port);

        current_io_port = io_port;
        current_control_port = control_port;
        current_device_drive = drive;
    }
}

bool disc_present(port_t io_port, port_t control_port, device_drive_t drive) {
    disc_select(io_port, control_port, drive);

    pio_write8(io_port, ATA_PIO_IO_SECTOR_COUNT_OFFSET, 0);
    pio_write8(io_port, ATA_PIO_IO_LBA_LOW_OFFSET, 0);
    pio_write8(io_port, ATA_PIO_IO_LBA_MID_OFFSET, 0);
    pio_write8(io_port, ATA_PIO_IO_LBA_HIGH_OFFSET, 0);

    pio_write8(io_port, ATA_PIO_IO_COMMAND_OFFSET, ATA_PIO_COMMAND_IDENTIFY);

    status_pack_t status = { .num = pio_read8(io_port, ATA_PIO_IO_STATUS_OFFSET) };

    if (status.num == 0) return false;

    while (status.bits.busy) status.num = pio_read8(io_port, ATA_PIO_IO_STATUS_OFFSET);

    if (
        pio_read8(io_port, ATA_PIO_IO_LBA_MID_OFFSET) != 0 ||
        pio_read8(io_port, ATA_PIO_IO_LBA_HIGH_OFFSET) != 0
    ) return false;

    while (!status.bits.data_ready && !status.bits.error) status.num = pio_read8(io_port, ATA_PIO_IO_STATUS_OFFSET);

    if (status.bits.error) return false;

    for (uint16_t i = 0; i < 256; i++) pio_read16(io_port, ATA_PIO_IO_DATA_OFFSET);

    return true;
}

uint32_t disc_read28(uint32_t lba, uint8_t sector_count, uint16_t * dest) {
    if (lba > 0xFFFFFF) return false;

    switch (current_device_drive) {
        case DEVICE_DRIVE_MASTER: {
            pio_write8(current_io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xE0 | ((lba >> 24) & 0xF));
        } break;
        case DEVICE_DRIVE_SLAVE: {
            pio_write8(current_io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xF0 | ((lba >> 24) & 0xF));
        } break;
    }

    read_status_long(current_io_port);

    pio_write8(current_io_port, ATA_PIO_IO_ERROR_OFFSET, 0);

    pio_write8(current_io_port, ATA_PIO_IO_SECTOR_COUNT_OFFSET, sector_count);

    pio_write8(current_io_port, ATA_PIO_IO_LBA_LOW_OFFSET, (lba >> 0) & 0xFF );
    pio_write8(current_io_port, ATA_PIO_IO_LBA_MID_OFFSET, (lba >> 8) & 0xFF);
    pio_write8(current_io_port, ATA_PIO_IO_LBA_HIGH_OFFSET, (lba >> 16) & 0xFF);

    pio_write8(current_io_port, ATA_PIO_IO_COMMAND_OFFSET, ATA_PIO_COMMAND_READ);

    for (uint8_t i = 0; i < sector_count; i++) {
        status_pack_t status = wait_data_ready();

        if (status.bits.error || status.bits.drive_fault) return i;

        for (uint16_t j = 0; j < 256; j++) {
            *dest = pio_read16(current_io_port, ATA_PIO_IO_DATA_OFFSET);

            dest++;
        }

        read_status_long(current_io_port);
    }

    return sector_count;
}

uint32_t disc_read48(uint64_t lba, uint16_t sector_count, uint16_t * dest) {

}

uint32_t disc_write28(uint32_t lba, uint8_t sector_count, const uint16_t * src) {
    if (lba > 0xFFFFFF) return false;

    switch (current_device_drive) {
        case DEVICE_DRIVE_MASTER: {
            pio_write8(current_io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xE0 | ((lba >> 24) & 0xF));
        } break;
        case DEVICE_DRIVE_SLAVE: {
            pio_write8(current_io_port, ATA_PIO_IO_DRIVE_HEAD_OFFSET, 0xF0 | ((lba >> 24) & 0xF));
        } break;
    }

    read_status_long(current_io_port);

    pio_write8(current_io_port, ATA_PIO_IO_ERROR_OFFSET, 0);

    pio_write8(current_io_port, ATA_PIO_IO_SECTOR_COUNT_OFFSET, sector_count);

    pio_write8(current_io_port, ATA_PIO_IO_LBA_LOW_OFFSET, (lba >> 0) & 0xFF );
    pio_write8(current_io_port, ATA_PIO_IO_LBA_MID_OFFSET, (lba >> 8) & 0xFF);
    pio_write8(current_io_port, ATA_PIO_IO_LBA_HIGH_OFFSET, (lba >> 16) & 0xFF);

    pio_write8(current_io_port, ATA_PIO_IO_COMMAND_OFFSET, ATA_PIO_COMMAND_WRITE);

    for (uint8_t i = 0; i < sector_count; i++) {
        status_pack_t status = wait_data_ready();

        if (status.bits.error || status.bits.drive_fault) return i;

        for (uint16_t j = 0; j < 256; j++) {
            pio_write16(current_io_port, ATA_PIO_IO_DATA_OFFSET, src[i * 256 + j]);
        }

        read_status_long(current_io_port);
    }

    cache_flush();

    return sector_count;
}

uint32_t disc_write48(uint32_t lba, uint8_t sector_count, const uint16_t * src) {

}

bool disc_read(uint64_t lba, uint64_t sector_count, void * dst) {
    if (disc_read28(lba, sector_count, dst) == sector_count) return true;
    else return false;
}

bool disc_write(uint64_t lba, uint64_t sector_count, const void * src) {
    if (disc_write28(lba, sector_count, src) == sector_count) return true;
    else return false;
}

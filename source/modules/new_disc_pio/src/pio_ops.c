#include <stdbool.h>

#include <pio_ops.h>

#include <sys/asm/in.h>
#include <sys/asm/out.h>
#include <sys/ata/pio.h>
#include <sys/ports.h>

static inline bool wait_ready(ata_pio_io_port_t * io_port) {
    volatile union { uint8_t uint8; ata_pio_status_t value; } status = { .uint8 = inb_ptr(&io_port->status), };
    while ((status.value.busy || !status.value.ready) && !status.value.error) status.uint8 = inb_ptr(&io_port->status);
    if (status.value.error) {
        return false;
    }

    return true;
}

enum {
    DEVICE_DETECT_DRIVE_ID_MASTER = 0xA0,
    DEVICE_DETECT_DRIVE_ID_SLAVE = 0xB0,
};
static inline bool device_detect(ata_pio_io_port_t * io_port, uint8_t drive_id) {
    outb_ptr(&io_port->drive_head, drive_id);
    outb_ptr(&io_port->lba_low, 0);
    outb_ptr(&io_port->lba_mid, 0);
    outb_ptr(&io_port->lba_high, 0);
    outb_ptr(&io_port->command, ATA_PIO_COMMAND_IDENTIFY);

    uint8_t first_status = inb_ptr(&io_port->status);

    if (first_status == 0) return false;

    union { ata_pio_status_t bits; uint8_t value; } status = { .value = inb_ptr(&io_port->status), };
    while (status.bits.busy) status.value = inb_ptr(&io_port->status);

    status.value = inb_ptr(&io_port->status);
    while (!status.bits.data_ready && !status.bits.error) status.value = inb_ptr(&io_port->status);

    if (status.bits.error) {
        return false;
    }

    return true;
}

static inline bool disc_reset(ata_pio_io_port_t * io_port, ata_pio_control_port_t * control_port) {
    outb_ptr(&control_port->device_control, ATA_PIO_DEVICE_CONTROL_SOFTWARE_RESET);
    outb_ptr(&control_port->device_control, 0);

    inb_ptr(&control_port->status);
    inb_ptr(&control_port->status);
    inb_ptr(&control_port->status);
    inb_ptr(&control_port->status);

    if (!wait_ready(io_port)) {
        return false;
    }

    return true;
}

enum {
    READ28_DRIVE_SELECT_MASTER = 0xE0,
    READ28_DRIVE_SELECT_SLAVE  = 0xF0,
};
static inline bool read28(ata_pio_io_port_t * io_port, uint8_t drive_select, uint64_t lba, uint16_t sector_count, uint16_t * dest) {
    if (lba > 0xFFFFFF) return false;

    outb_ptr(&io_port->drive_head, drive_select | ((lba >> 24) & 0xF));

    outb_ptr(&io_port->sector_count, sector_count);

    outb_ptr(&io_port->lba_low, lba);
    outb_ptr(&io_port->lba_mid, lba >> 8);
    outb_ptr(&io_port->lba_high, lba >> 16);

    outb_ptr(&io_port->command, ATA_PIO_COMMAND_READ);

    inb_ptr(&io_port->status);
    inb_ptr(&io_port->status);
    inb_ptr(&io_port->status);
    inb_ptr(&io_port->status);

    for (int i = 0; i < sector_count; i++) {
        for (int j = 0; j < 256; j++) {
            while (1) {
                union { uint8_t uint8; ata_pio_status_t value; } status = { .uint8 = inb_ptr(&io_port->status), };

                if (status.value.error) {
                    return false;
                }

                if (status.value.data_ready) break;
            }

            *dest = inw_ptr(&io_port->data);

            dest++;
        }
    }
    if (!wait_ready(io_port)) {
        return false;
    }

    return true;
}
static inline bool write28(ata_pio_io_port_t * io_port, uint8_t drive_select, uint64_t lba, uint16_t sector_count, const uint16_t * src) {
    if (lba > 0xFFFFFF) return false;

    outb_ptr(&io_port->drive_head, drive_select | ((lba >> 24) & 0xF));

    outb_ptr(&io_port->sector_count, sector_count);

    outb_ptr(&io_port->lba_low, lba);
    outb_ptr(&io_port->lba_mid, lba >> 8);
    outb_ptr(&io_port->lba_high, lba >> 16);

    outb_ptr(&io_port->command, ATA_PIO_COMMAND_WRITE);

    for (int i = 0; i < sector_count; i++) {
        if (!wait_ready(io_port)) {
            return false;
        }

        for (int j = 0; j < 256; j++) {
            outw_ptr(&io_port->data, *src);

            src++;

            if (!wait_ready(io_port)) {
                return false;
            }
        }

        outb_ptr(&io_port->command, ATA_PIO_COMMAND_CACHE_FLUSH);
    }

    if (!wait_ready(io_port)) {
        return false;
    }

    return true;
}

enum {
    READ48_DRIVE_SELECT_MASTER = 0x40,
    READ48_DRIVE_SELECT_SLAVE  = 0x50,
};
static inline bool read48(ata_pio_io_port_t * io_port, uint8_t drive_select, uint64_t lba, uint16_t sector_count, uint16_t * dest) {
    if (lba > 0xFFFFFFFFFFFF) return false;

    outb_ptr(&io_port->drive_head, drive_select);

    outb_ptr(&io_port->sector_count, sector_count >> 8);
    outb_ptr(&io_port->lba_low, lba >> 24);
    outb_ptr(&io_port->lba_mid, lba >> 32);
    outb_ptr(&io_port->lba_high, lba >> 40);
    outb_ptr(&io_port->sector_count, sector_count & 0xFF);
    outb_ptr(&io_port->lba_low, lba >> 0);
    outb_ptr(&io_port->lba_mid, lba >> 8);
    outb_ptr(&io_port->lba_high, lba >> 16);
    outb_ptr(&io_port->command, ATA_PIO_COMMAND_READ_EXT);

    for (int i = 0; i < sector_count; i++) {
        if (!wait_ready(io_port)) {
            return false;
        }

        for (int j = 0; j < 256; j++) {
            *dest = inw_ptr(&io_port->data);

            dest++;

            if (!wait_ready(io_port)) {
                return false;
            }
        }
    }

    if (!wait_ready(io_port)) {
        return false;
    }

    return true;
}

static inline bool write48(ata_pio_io_port_t * io_port, uint8_t drive_select, uint64_t lba, uint16_t sector_count, const uint16_t * src) {
    if (lba > 0xFFFFFFFFFFFF) return false;

    outb_ptr(&io_port->drive_head, drive_select);

    outb_ptr(&io_port->sector_count, sector_count >> 8);
    outb_ptr(&io_port->lba_low, lba >> 24);
    outb_ptr(&io_port->lba_mid, lba >> 32);
    outb_ptr(&io_port->lba_high, lba >> 40);
    outb_ptr(&io_port->sector_count, sector_count & 0xFF);
    outb_ptr(&io_port->lba_low, lba >> 0);
    outb_ptr(&io_port->lba_mid, lba >> 8);
    outb_ptr(&io_port->lba_high, lba >> 16);
    outb_ptr(&io_port->command, ATA_PIO_COMMAND_WRITE_EXT);

    for (int i = 0; i < sector_count; i++) {
        if (!wait_ready(io_port)) {
            return false;
        }

        for (int j = 0; j < 256; j++) {
            outw_ptr(&io_port->data, *src);

            src++;

            if (!wait_ready(io_port)) {
                return false;
            }
        }
    }

    if (!wait_ready(io_port)) {
        return false;
    }

    return true;
}

bool disc_read(uint64_t lba, uint64_t sector_count, void * dst) {
    ata_pio_io_port_t * io_port = device_io_port();

    if (lba > 0xFFFFFF || sector_count > 0xFF) {
        return read48(
            io_port,
            disc_devices[selected_device].drive == DEVICE_DRIVE_MASTER ? READ48_DRIVE_SELECT_MASTER : READ48_DRIVE_SELECT_SLAVE,
            lba,
            sector_count,
            dst
        );
    }
    else {
        return read28(
            io_port,
            disc_devices[selected_device].drive == DEVICE_DRIVE_MASTER ? READ28_DRIVE_SELECT_MASTER : READ28_DRIVE_SELECT_SLAVE,
            lba,
            sector_count,
            dst
        );
    }
}

bool disc_write(uint64_t lba, uint64_t sector_count, const void * src) {
    ata_pio_io_port_t * io_port = device_io_port();

    if (lba > 0xFFFFFF || sector_count > 0xFF) {
        return write48(
            io_port,
            disc_devices[selected_device].drive == DEVICE_DRIVE_MASTER ? READ48_DRIVE_SELECT_MASTER : READ48_DRIVE_SELECT_SLAVE,
            lba,
            sector_count,
            src
        );
    }
    else {
        return write28(
            io_port,
            disc_devices[selected_device].drive == DEVICE_DRIVE_MASTER ? READ28_DRIVE_SELECT_MASTER : READ28_DRIVE_SELECT_SLAVE,
            lba,
            sector_count,
            src
        );
    }
}
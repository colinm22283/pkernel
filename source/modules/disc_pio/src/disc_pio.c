#include <stddef.h>

#include <device/devfs.h>
#include <device/device.h>

#include <io/arbitrator.h>

#include <util/string/strlen.h>
#include <util/string/strcpy.h>

#include <sys/asm/in.h>
#include <sys/asm/out.h>
#include <sys/ata/pio.h>
#include <sys/ports.h>

#define PRIMARY_IO_PORT ((ata_pio_io_port_t *) ATA_PIO_PRIMARY)
#define SECONDARY_IO_PORT ((ata_pio_io_port_t *) ATA_PIO_SECONDARY)
#define PRIMARY_CONTROL_PORT ((ata_pio_control_port_t *) ATA_PIO_PRIMARY_CONTROL)
#define SECONDARY_CONTROL_PORT ((ata_pio_control_port_t *) ATA_PIO_SECONDARY_CONTROL)

device_t * volatile devices[4];
devfs_entry_t * devfs_nodes[4];

uint8_t device_indexes[4];

enum { DEVICE_BUS_PRIMARY, DEVICE_BUS_SECONDARY };
enum { DEVICE_DRIVE_MASTER, DEVICE_DRIVE_SLAVE };
typedef struct {
    uint8_t bus;
    uint8_t drive;
} disc_device_t;

enum {
    DRIVER_OK,
    DRIVER_ERROR_ADDRESS_MARK_NOT_FOUND,
    DRIVER_ERROR_TRACK_ZERO_NOT_FOUND,
    DRIVER_ERROR_ABORTED_COMMAND,
    DRIVER_ERROR_MEDIA_CHANGE_REQUEST,
    DRIVER_ERROR_ID_NOT_FOUND,
    DRIVER_ERROR_MEDIA_CHANGED,
    DRIVER_ERROR_UNCORRECTABLE_DATA_ERROR,
    DRIVER_ERROR_BAD_BLOCK_DETECTED,
    DRIVER_ERROR_UNKNOWN,
};

volatile uint16_t dev_count;
volatile uint8_t selected_device;
volatile disc_device_t disc_devices[4];

static inline bool wait_ready(ata_pio_io_port_t * io_port) {
    volatile union { uint8_t uint8; ata_pio_status_t value; } status = { .uint8 = inb_ptr(&io_port->status), };
    while ((status.value.busy || !status.value.ready) && !status.value.error) status.uint8 = inb_ptr(&io_port->status);
    if (status.value.error) {
        return false;
    }

    return true;
}

static inline ata_pio_io_port_t * device_io_port() {
    switch (disc_devices[selected_device].bus) {
        case DEVICE_BUS_PRIMARY: return PRIMARY_IO_PORT;
        case DEVICE_BUS_SECONDARY: return SECONDARY_IO_PORT;
        default: return NULL;
    }
}

static inline ata_pio_control_port_t * device_control_port() {
    switch (disc_devices[selected_device].bus) {
        case DEVICE_BUS_PRIMARY: return PRIMARY_CONTROL_PORT;
        case DEVICE_BUS_SECONDARY: return SECONDARY_CONTROL_PORT;
        default: return NULL;
    }
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

static inline void init_device_count() {
    dev_count = 0;

    bool primary_present = inb_ptr(&PRIMARY_IO_PORT->status) != 0xFF;
    bool secondary_present = inb_ptr(&SECONDARY_IO_PORT->status) != 0xFF;

    if (primary_present) {
        if (device_detect(PRIMARY_IO_PORT, DEVICE_DETECT_DRIVE_ID_MASTER)) {
            disc_devices[dev_count++] = (disc_device_t) {
                .bus = DEVICE_BUS_PRIMARY,
                .drive = DEVICE_DRIVE_MASTER,
            };
        }

        // if (device_detect(PRIMARY_IO_PORT, DEVICE_DETECT_DRIVE_ID_SLAVE)) {
        //     disc_devices[dev_count++] = (disc_device_t) {
        //         .bus = DEVICE_BUS_PRIMARY,
        //         .drive = DEVICE_DRIVE_SLAVE,
        //     };
        // }
    }

    // if (secondary_present) {
    //     if (device_detect(SECONDARY_IO_PORT, DEVICE_DETECT_DRIVE_ID_MASTER)) {
    //         disc_devices[dev_count++] = (disc_device_t) {
    //             .bus = DEVICE_BUS_SECONDARY,
    //             .drive = DEVICE_DRIVE_MASTER,
    //         };
    //     }
    //     if (device_detect(SECONDARY_IO_PORT, DEVICE_DETECT_DRIVE_ID_SLAVE)) {
    //         disc_devices[dev_count++] = (disc_device_t) {
    //             .bus = DEVICE_BUS_SECONDARY,
    //             .drive = DEVICE_DRIVE_SLAVE,
    //         };
    //     }
    // }
}

bool device_count(uint64_t * count) {
    *count = dev_count;

    return true;
}

bool select_device(uint64_t device) {
    if (device >= dev_count) return false;

    selected_device = device;

    return disc_reset(device_io_port(), device_control_port());
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

uint64_t write(device_t * device, const char * buffer, uint64_t block_size, uint64_t block_offset) {
    uint8_t requested_device = *(uint8_t *) device->private;

    if (requested_device != selected_device) select_device(requested_device);

    if (!disc_write(block_offset, block_size, buffer)) {
        return 0;
    }

    return block_size;
}

uint64_t read(device_t * device, char * buffer, uint64_t block_size, uint64_t block_offset) {
    uint8_t requested_device = *(uint8_t *) device->private;

    if (requested_device != selected_device) select_device(requested_device);

    if (!disc_read(block_offset, block_size, buffer)) {
        return 0;
    }

    return block_size;
}

bool init(void) {
    if (!io_arbitrator_reserve(ATA_PIO_PRIMARY)) return false;
    if (!io_arbitrator_reserve(ATA_PIO_SECONDARY)) return false;
    if (!io_arbitrator_reserve(ATA_PIO_PRIMARY_CONTROL)) return false;
    if (!io_arbitrator_reserve(ATA_PIO_SECONDARY_CONTROL)) return false;

    init_device_count();
    selected_device = 0;

    select_device(0);

    for (uint64_t i = 0; i < dev_count; i++) {
        device_block_operations_t operations = {
            .write = write,
            .read = read,
        };
        device_block_data_t data = {
            .block_size = 512,
        };

        char name[6] = "disc0";
        name[4] = (char) ('0' + i);

        device_indexes[i] = i;

        devices[i] = device_create_block(name, &device_indexes[i], &operations, &data);
        devfs_nodes[i] = devfs_register(devices[i]);
    }

    return true;
}

bool free(void) {
    for (uint64_t i = 0; i < dev_count; i++) {
        devfs_remove(devfs_nodes[i]);
        device_remove(devices[i]);
    }

    return true;
}


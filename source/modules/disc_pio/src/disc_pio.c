#include <stddef.h>

#include <devfs/devfs.h>
#include <device/device.h>

#include <io/arbitrator.h>

#include <pci/pci.h>

#include <pio_ops.h>
#include <disc_pio.h>

#include <util/heap/heap.h>
#include <util/string/writestr.h>

#include <sys/ata/pio.h>
#include <sys/ports.h>
#include <sys/halt.h>

#include <debug/vga_print.h>

uint64_t block_write(struct device_s * device, const char * buffer, uint64_t block_size, uint64_t block_offset);
uint64_t block_read(struct device_s * device, char * buffer, uint64_t block_size, uint64_t block_offset);

uint64_t ide_device_count = 0;
ide_device_t ** ide_devices = NULL;

device_block_operations_t block_ops = {
    .write = block_write,
    .read = block_read,
};

device_block_data_t block_data = {
    .block_size = 512,
};

pci_watcher_t * pci_watcher;

void add_ide_device(port_t io_port, port_t control_port, bool is_master);

pci_probe_result_t pci_probe(pci_device_t * device, void * private) {
    if (device->class == PCI_CLASS_MASS_STORAGE_CONTROLLER && device->subclass == PCI_SUBCLASS_IDE_CONTROLLER) {
        debug_print("Found IDE Controller\n");

        {
            port_t disc_io_port = pci_read_bar(device->address, 0) & ~0b11;
            port_t disc_control_port = pci_read_bar(device->address, 1) & ~0b11;
            if (disc_io_port == 0) disc_io_port = ATA_PIO_PRIMARY;
            if (disc_control_port == 0) disc_control_port = ATA_PIO_PRIMARY_CONTROL;

            if (!io_arbitrator_reserve(disc_io_port)) return PCI_IGNORE;
            if (!io_arbitrator_reserve(disc_control_port)) return PCI_IGNORE;

            add_ide_device(disc_io_port, disc_control_port, true);
            add_ide_device(disc_io_port, disc_control_port, false);
        }

        {
            port_t disc_io_port = pci_read_bar(device->address, 2) & ~0b11;
            port_t disc_control_port = pci_read_bar(device->address, 3) & ~0b11;
            if (disc_io_port == 0) disc_io_port = ATA_PIO_SECONDARY;
            if (disc_control_port == 0) disc_control_port = ATA_PIO_SECONDARY_CONTROL;

            if (!io_arbitrator_reserve(disc_io_port)) return PCI_IGNORE;
            if (!io_arbitrator_reserve(disc_control_port)) return PCI_IGNORE;

            add_ide_device(disc_io_port, disc_control_port, true);
            add_ide_device(disc_io_port, disc_control_port, false);
        }

        return PCI_BIND;
    }

    return PCI_IGNORE;
}

void add_ide_device(port_t io_port, port_t control_port, bool is_master) {
    if (disc_present(io_port, control_port, is_master ? DEVICE_DRIVE_MASTER : DEVICE_DRIVE_SLAVE)) {
        ide_devices = heap_realloc(ide_devices, (ide_device_count + 1) * sizeof(ide_device_t *));
        ide_devices[ide_device_count] = heap_alloc_debug(sizeof(ide_device_t), "disc_pio ide_device");
        ide_device_t * ide_device = ide_devices[ide_device_count];
        ide_device_count++;

        ide_device->io_port = io_port;
        ide_device->control_port = control_port;
        ide_device->is_master = is_master;

        char name[30] = "disc";
        name[4 + writestr(name + 4, 25, 0, ide_device_count - 1)] = '\0';

        device_t * device = device_create_block(name, ide_device, &block_ops, &block_data);
        ide_device->devfs_entry = devfs_register(device);
    }
}

error_number_t init(void) {
    ide_device_count = 0;
    ide_devices = heap_alloc_debug(1, "disc_pio ide_devices");

    pci_watcher = pci_watch(pci_probe, NULL);

    return ERROR_OK;
}

error_number_t free(void) {
    pci_unwatch(pci_watcher);

    return ERROR_OK;
}

uint64_t block_write(device_t * device, const char * buffer, uint64_t block_size, uint64_t block_offset) {
    // vga_print("Write\n");

    ide_device_t * ide_device = device->private;

    // vga_print("Select device\n");

    disc_select(ide_device->io_port, ide_device->control_port, ide_device->is_master ? DEVICE_DRIVE_MASTER : DEVICE_DRIVE_SLAVE);

    // vga_print("Begin Write\n");

    if (!disc_write(block_offset, block_size, buffer)) return 0;

    // vga_print("Done\n");

    return block_size;
}

uint64_t block_read(device_t * device, char * buffer, uint64_t block_size, uint64_t block_offset) {
    ide_device_t * ide_device = device->private;

    disc_select(ide_device->io_port, ide_device->control_port, ide_device->is_master ? DEVICE_DRIVE_MASTER : DEVICE_DRIVE_SLAVE);

    if (!disc_read(block_offset, block_size, buffer)) return 0;

    return block_size;
}

MODULE_NAME("disc_pio");
MODULE_DEPS_NONE();

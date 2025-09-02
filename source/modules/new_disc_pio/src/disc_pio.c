#include <stddef.h>

#include <device/devfs.h>
#include <device/device.h>

#include <io/arbitrator.h>

#include <pci/pci.h>

#include <pio_ops.h>

#include <sys/ata/pio.h>
#include <sys/ports.h>

pci_probe_result_t pci_probe(pci_device_t * device) {
    if (device->class == PCI_CLASS_IDE_CONTROLLER && device->subclass == PCI_SUBCLASS_IDE_CONTROLLER) {
        port_t disc_io_port = pci_read_bar(device->address, 0) & ~0b11;
        port_t disc_control_port = pci_read_bar(device->address, 1) & ~0b11;
        if (disc_io_port == 0) disc_io_port = ATA_PIO_PRIMARY;
        if (disc_control_port == 0) disc_control_port = ATA_PIO_PRIMARY_CONTROL;

        if (!io_arbitrator_reserve(disc_io_port)) return false;
        if (!io_arbitrator_reserve(disc_control_port)) return false;

        return PCI_BIND;
    }

    return PCI_IGNORE;
}

bool init(void) {


    return true;
}

bool free(void) {
    return true;
}


#pragma once

#include <stdbool.h>

#include <mod_defs.h>

#include <sys/pci/pci.h>

struct pci_watcher_s;

typedef struct {
    struct pci_watcher_s * binder;

    pci_address_t address;

    uint8_t device_id;
    uint8_t vendor_id;

    uint8_t class;
    uint8_t subclass;

    uint8_t prog_if;

    uint8_t rev_id;

    uint8_t bist_id;

    uint8_t header_type;

    uint8_t latency;
    uint8_t cache_line_size;
} pci_device_t;

typedef enum {
    PCI_IGNORE,
    PCI_BIND,
} pci_probe_result_t;

typedef pci_probe_result_t (pci_probe_t)(pci_device_t * pci_device, void * private);

typedef struct pci_watcher_s {
    pci_probe_t * probe;
    void * private;

    uint64_t bound_count;
    pci_device_t ** bound_devices;

    struct pci_watcher_s * next;
    struct pci_watcher_s * prev;
} pci_watcher_t;

extern uint64_t pci_device_count;
extern pci_device_t ** pci_devices;

void pci_init(void);

pci_watcher_t * pci_watch(pci_probe_t * probe, void * private);
void pci_unwatch(pci_watcher_t * probe);


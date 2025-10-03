#include <stdint.h>
#include <stddef.h>

#include <pci/pci.h>

#include <util/heap/heap.h>

uint64_t pci_device_count;
pci_device_t ** pci_devices;

pci_watcher_t pci_watcher_head, pci_watcher_tail;

static inline void add_pci_device(pci_address_t address) {
    pci_devices[pci_device_count] = heap_alloc(sizeof(pci_device_t));
    pci_device_t * pci_device = pci_devices[pci_device_count];

    pci_device->binder = NULL;

    pci_device->address = address;

    uint16_t class_pair = pci_get_class_pair(address);

    // TODO
    pci_device->device_id       = 0;
    pci_device->vendor_id       = 0;
    pci_device->class           = (class_pair >> 8) & 0xFF;
    pci_device->subclass        = (class_pair >> 0) & 0xFF;
    pci_device->prog_if         = 0;
    pci_device->rev_id          = 0;
    pci_device->bist_id         = 0;
    pci_device->header_type     = 0;
    pci_device->latency         = 0;
    pci_device->cache_line_size = 0;

    pci_device_count++;
    pci_devices = heap_realloc(pci_devices, (pci_device_count + 1) * sizeof(pci_device_t));
}

static inline void pci_bind(pci_watcher_t * watcher, pci_device_t * device) {
    device->binder = watcher;

    watcher->bound_devices[watcher->bound_count] = device;

    watcher->bound_count++;
    watcher->bound_devices = heap_realloc(watcher->bound_devices, (watcher->bound_count + 1) * sizeof(pci_watcher_t));
}

void pci_init(void) {
    pci_device_count = 0;
    pci_devices = heap_alloc(sizeof(pci_device_t));

    pci_watcher_head.next = &pci_watcher_tail;
    pci_watcher_head.prev = NULL;
    pci_watcher_tail.next = NULL;
    pci_watcher_tail.prev = &pci_watcher_head;

    for (uint64_t bus = 0; bus < PCI_BUS_MAX; bus++) {
        for (uint64_t slot = 0; slot < PCI_SLOT_MAX; slot++) {
            pci_address_t base_address = PCI_ADDRESS(bus, slot, 0);

            if (pci_exists(base_address)) {
                uint64_t func_max;

                if (pci_is_multifunction(base_address)) func_max = PCI_FUNC_MAX;
                else func_max = 1;

                for (uint64_t func = 0; func < func_max; func++) {
                    pci_address_t address = PCI_ADDRESS(bus, slot, func);

                    if (pci_exists(address)) {
                        add_pci_device(address);
                    }
                }
            }
        }
    }
}

pci_watcher_t * pci_watch(pci_probe_t * probe, void * private) {
    pci_watcher_t * watcher = heap_alloc(sizeof(pci_watcher_t));

    watcher->probe = probe;
    watcher->private = private;

    watcher->bound_count = 0;
    watcher->bound_devices = heap_alloc(sizeof(pci_device_t *));

    watcher->next = pci_watcher_head.next;
    watcher->prev = &pci_watcher_head;
    pci_watcher_head.next->prev = watcher;
    pci_watcher_head.next = watcher;

    for (uint64_t i = 0; i < pci_device_count; i++) {
        if (pci_devices[i]->binder == NULL) {
            if (watcher->probe(pci_devices[i], watcher->private) == PCI_BIND) {
                pci_bind(watcher, pci_devices[i]);
            }
        }
    }

    return watcher;
}

void pci_unwatch(pci_watcher_t * watcher) {
    for (uint64_t i = 0; i < watcher->bound_count; i++) {
        watcher->bound_devices[i]->binder = NULL;
    }

    heap_free(watcher->bound_devices);

    watcher->prev->next = watcher->next;
    watcher->next->prev = watcher->prev;
}
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <sys/asm/out.h>
#include <sys/asm/in.h>

#define PCI_BUS_MAX (256)
#define PCI_SLOT_MAX (32)
#define PCI_FUNC_MAX (8)

#define PCI_CLASS_IDE_CONTROLLER (0x01)
#define PCI_SUBCLASS_IDE_CONTROLLER (0x01)
#define PCI_CLASS_PAIR_IDE_CONTROLLER (0x0101)

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
} pci_address_t;

#define PCI_ADDRESS(b, s, f) ((pci_address_t) { .bus = (b), .slot = (s), .func = (f), })

static inline uint32_t pci_read_long(pci_address_t pci, uint8_t offset) {
    uint32_t lbus  = (uint32_t) pci.bus;
    uint32_t lslot = (uint32_t) pci.slot;
    uint32_t lfunc = (uint32_t) pci.func;

    uint32_t address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));
    outl(0xCF8, address);
    return (uint32_t) (inl(0xCFC) >> ((offset & 2) * 8));
}

static inline uint16_t pci_read_word(pci_address_t pci, uint8_t offset) {
    uint32_t lbus  = (uint32_t) pci.bus;
    uint32_t lslot = (uint32_t) pci.slot;
    uint32_t lfunc = (uint32_t) pci.func;

    uint32_t address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));
    outl(0xCF8, address);
    return (uint16_t) ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

static inline uint16_t pci_read_byte(pci_address_t pci, uint8_t offset) {
    uint32_t lbus  = (uint32_t) pci.bus;
    uint32_t lslot = (uint32_t) pci.slot;
    uint32_t lfunc = (uint32_t) pci.func;

    uint32_t address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));
    outl(0xCF8, address);
    return (uint16_t) ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFF);
}

static inline bool pci_exists(pci_address_t pci) {
    if (pci_read_word(pci, 0) == 0xFFFF) return false;
    else return true;
}

static inline uint32_t pci_get_class_pair(pci_address_t pci) {
    return pci_read_word(pci, 10);
}

static inline bool pci_is_multifunction(pci_address_t pci) {
    pci.func = 0;
    return (pci_read_byte(pci, 0xE) & 0x80) != 0;
}

static inline uint32_t pci_read_bar(pci_address_t pci, uint8_t bar) {
    return pci_read_long(pci, 0x10 + 4 * bar);
}

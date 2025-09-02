#pragma once

#include <stdint.h>

#include <sys/port.h>

//#define outb(byte, port) do { asm volatile ("outb %0, %1" : : "r" ((uint8_t) byte), "i" (port)); } while (0)

static inline void outb(port_t port, uint8_t byte) {
    asm volatile ("outb %b0, %w1" : : "a" (byte), "Nd" (port) : "memory");
}

static inline void outb_ptr(const structure_port_t * port, uint8_t byte) {
    asm volatile ("outb %b0, %w1" : : "a" (byte), "Nd" ((port_t) (intptr_t) port) : "memory");
}

static inline void outw(port_t port, uint16_t word) {
    asm volatile ("outw %w0, %w1" : : "a" (word), "Nd" (port) : "memory");
}

static inline void outw_ptr(const structure_port_t * port, uint16_t word) {
    asm volatile ("outw %w0, %w1" : : "a" (word), "Nd" ((port_t) (intptr_t) port) : "memory");
}

static inline void outl(port_t port, uint32_t word) {
    asm volatile ("outl %0, %w1" : : "a" (word), "Nd" (port) : "memory");
}

static inline void outl_ptr(const structure_port_t * port, uint32_t word) {
    asm volatile ("outl %0, %w1" : : "a" (word), "Nd" ((port_t) (intptr_t) port) : "memory");
}
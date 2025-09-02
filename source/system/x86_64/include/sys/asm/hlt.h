#pragma once

static inline void hlt() {
    asm volatile ("hlt");
}
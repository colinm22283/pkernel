#pragma once

static inline void sti() {
    asm volatile ("sti");
}
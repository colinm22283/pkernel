#pragma once

static inline void invalidate_page(void * vaddr) {
    asm volatile ("invlpg (%0)" : : "r" (vaddr) : "memory");
}
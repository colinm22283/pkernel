#include <sys/interrupt/interrupt.h>

#include <sys/gdt/gdt.h>

#include <sys/paging/init.h>
#include <../../../include/paging/bitmap.h>

void sys_setup_phase1(void) {
    gdt_init();
}

void sys_setup_phase2(void) {
    interrupt_init();

    sys_paging_init();

    paging_bitmap_init();
}
#include <sys/interrupt/interrupt.h>

#include <sys/gdt/gdt.h>

void sys_setup(void) {
    gdt_init();
}

void sys_setup_phase2(void) {
    interrupt_init();
}
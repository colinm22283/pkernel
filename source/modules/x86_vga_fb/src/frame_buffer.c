#include <stddef.h>

#include <mod_defs.h>

int init(void) {
    return 0;
}

int free(void) {
    return 0;
}

MODULE_NAME("x86_vga_fb");
MODULE_DEPS("devfs");


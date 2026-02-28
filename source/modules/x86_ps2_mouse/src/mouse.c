#include <interrupt/interrupt_registry.h>

#include <sys/interrupt/interrupt_code.h>

#include <errno.h>

#include <mod_defs.h>

void mouse_handler(interrupt_code_t channel, task_state_record_t * tsr, void * interrupt_code) {
}

int init() {
    if (!interrupt_registry_register((interrupt_code_t) IC_MOUSE, mouse_handler)) return ERROR_INT_UNAVAIL;

    return 0;
}

int free() {
    return 0;
}

MODULE_NAME("x86_ps2_mouse");
MODULE_DEPS("devfs");

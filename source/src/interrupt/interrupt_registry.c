#include <stdint.h>
#include <stddef.h>

#include <interrupt/interrupt_registry.h>

#include <sys/interrupt/interrupt_code.h>

#include <entry_error.h>

typedef struct {
    bool active;
    interrupt_handler_t * handler;
} interrupt_registry_node_t;

interrupt_registry_node_t interrupt_registry_nodes[IC_MAX];

void interrupt_registry_init(void) {
    for (uint64_t i = 0; i < IC_MAX; i++) {
        interrupt_registry_nodes[i].active = false;
        interrupt_registry_nodes[i].handler = NULL;
    }
}

bool interrupt_registry_register(interrupt_code_t channel, interrupt_handler_t * handler) {
    if (interrupt_registry_nodes[channel].active) return false;

    interrupt_registry_nodes[channel].handler = handler;

    interrupt_registry_nodes[channel].active = true;

    return true;
}

bool interrupt_registry_free(interrupt_code_t channel) {
    if (!interrupt_registry_nodes[channel].active) return false;

    interrupt_registry_nodes[channel].handler = NULL;

    interrupt_registry_nodes[channel].active = false;

    return true;
}

void interrupt_registry_invoke(interrupt_code_t channel, interrupt_state_record_t * isr, void * interrupt_code) {
    if (interrupt_registry_nodes[channel].active) {
        interrupt_registry_nodes[channel].handler(channel, isr, interrupt_code);
    }
}
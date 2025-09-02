#include <stdint.h>
#include <stddef.h>

#include <interrupt/interrupt_registry.h>

#include <entry_error.h>

typedef struct {
    bool active;
    void * cookie;
    interrupt_handler_t handler;
} interrupt_registry_node_t;

interrupt_registry_node_t interrupt_registry_nodes[IC_LENGTH];

void interrupt_registry_init(void) {
    for (uint64_t i = 0; i < IC_LENGTH; i++) {
        interrupt_registry_nodes[i].active = false;
        interrupt_registry_nodes[i].handler = NULL;
        interrupt_registry_nodes[i].cookie = NULL;
    }
}

bool interrupt_registry_register(interrupt_channel_t channel, interrupt_handler_t handler, void * cookie) {
    if (interrupt_registry_nodes[channel].active) return false;

    interrupt_registry_nodes[channel].handler = handler;
    interrupt_registry_nodes[channel].cookie = cookie;

    interrupt_registry_nodes[channel].active = true;

    return true;
}

bool interrupt_registry_free(interrupt_channel_t channel) {
    if (!interrupt_registry_nodes[channel].active) return false;

    interrupt_registry_nodes[channel].handler = NULL;
    interrupt_registry_nodes[channel].cookie = NULL;

    interrupt_registry_nodes[channel].active = false;

    return true;
}

void interrupt_registry_invoke(interrupt_channel_t channel) {
    if (interrupt_registry_nodes[channel].active) {
        interrupt_registry_nodes[channel].handler(channel, interrupt_registry_nodes[channel].cookie);
    }
}
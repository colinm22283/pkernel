#include <interrupt/interrupt_registry.h>

void device_not_avail_handler(void) {
    interrupt_registry_invoke(IC_DEVICE_NOT_AVAIL);
}

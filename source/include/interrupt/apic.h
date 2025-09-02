#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <sys/apic/apic.h>

extern apic_t * interrupt_apic;

bool interrupt_apic_available(void);
void interrupt_apic_init(void);

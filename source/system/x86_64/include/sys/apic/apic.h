#pragma once

#include <stdint.h>

#include <sys/msr/write_msr.h>
#include <sys/msr/msr_numbers.h>

#include <defs.h>

#define APIC_BASE_MSR_BSP 0x100
#define APIC_BASE_MSR_ENABLE 0x800

#define APIC_REGISTER(name)                            \
    uint32_t name;                                     \
    uint8_t _##name##_reserved[16 - sizeof(uint32_t)]

typedef struct __PACKED {
    APIC_REGISTER(_reserved0);
    APIC_REGISTER(_reserved1);

    APIC_REGISTER(lapic_id);
    APIC_REGISTER(lapic_version);

    APIC_REGISTER(_reserved2);
    APIC_REGISTER(_reserved3);
    APIC_REGISTER(_reserved4);
    APIC_REGISTER(_reserved5);

    APIC_REGISTER(task_priority);
    APIC_REGISTER(arbitration_priority);
    APIC_REGISTER(processor_priority);

    APIC_REGISTER(end_of_interrupt);

    APIC_REGISTER(remote_read);

    APIC_REGISTER(logical_destination);
    APIC_REGISTER(destination_format);

    APIC_REGISTER(spurious_interrupt_vector);

    APIC_REGISTER(isr0);
    APIC_REGISTER(isr1);
    APIC_REGISTER(isr2);
    APIC_REGISTER(isr3);
    APIC_REGISTER(isr4);
    APIC_REGISTER(isr5);
    APIC_REGISTER(isr6);
    APIC_REGISTER(isr7);

    APIC_REGISTER(tmr0);
    APIC_REGISTER(tmr1);
    APIC_REGISTER(tmr2);
    APIC_REGISTER(tmr3);
    APIC_REGISTER(tmr4);
    APIC_REGISTER(tmr5);
    APIC_REGISTER(tmr6);
    APIC_REGISTER(tmr7);

    APIC_REGISTER(irr0);
    APIC_REGISTER(irr1);
    APIC_REGISTER(irr2);
    APIC_REGISTER(irr3);
    APIC_REGISTER(irr4);
    APIC_REGISTER(irr5);
    APIC_REGISTER(irr6);
    APIC_REGISTER(irr7);

    APIC_REGISTER(error_status);

    APIC_REGISTER(_reserved6);
    APIC_REGISTER(_reserved7);
    APIC_REGISTER(_reserved8);
    APIC_REGISTER(_reserved9);
    APIC_REGISTER(_reserved10);
    APIC_REGISTER(_reserved11);

    APIC_REGISTER(cmci);

    APIC_REGISTER(icr0);
    APIC_REGISTER(icr1);

    APIC_REGISTER(lvt_timer);
    APIC_REGISTER(lvt_thermal);
    APIC_REGISTER(lvt_performance_monitor);
    APIC_REGISTER(lvt_lint0);
    APIC_REGISTER(lvt_lint1);
    APIC_REGISTER(lvt_error);

    APIC_REGISTER(timer_initial_count);
    APIC_REGISTER(timer_current_count);

    APIC_REGISTER(_reserved12);
    APIC_REGISTER(_reserved13);
    APIC_REGISTER(_reserved14);
    APIC_REGISTER(_reserved15);

    APIC_REGISTER(timer_divide);

    APIC_REGISTER(_reserved16);
} apic_t;

/*static inline void set_apic_paddr(uint64_t paddr) {
    uint32_t eax = (paddr & 0xfffff0000) | APIC_BASE_MSR_ENABLE;
    uint32_t edx = (paddr >> 32) & 0x0f;

    write_msr(MSR_IA32_APIC, (paddr & 0xffffffffffff0000UL) | APIC_BASE_MSR_ENABLE);
}*/

#undef APIC_REGISTER

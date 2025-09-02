#include <interrupt/apic.h>

#include <memory/physical_reservations.h>

#include <paging/mapper.h>
#include <paging/_virtual_allocator.h>

#include <util/math/div_up.h>

#include <sys/cpuid.h>

apic_t * interrupt_apic;

bool interrupt_apic_available(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(CPUID_GETFEATURES, &eax, &ebx, &ecx, &edx);

    return edx & CPUID_FEAT_EDX_APIC;
}

void interrupt_apic_init(void) {
//    set_apic_paddr(APIC_PADDR);
//
//    uint64_t size_pages = DIV_UP(sizeof(apic_t), 0x1000);
//
//    interrupt_apic = paging_valloc_alloc(size_pages);
//
//    paging_mapping_t apic_mapping;
//    paging_map(&apic_mapping, APIC_PADDR, interrupt_apic, size_pages);
//
//    interrupt_apic->spurious_interrupt_vector |= (1 << 8);
}
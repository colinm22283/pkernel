#include <paging/tables.h>

pml4t64_t paging_kernel_pml4t;
pdpt64_t  paging_kernel_pdpt;
pdt64_t   paging_kernel_pdt;
pt64_t    paging_kernel_pt;

pdt64_t   paging_identity_pdt;
pt64_t    paging_identity_pt;

pdpt64_t  paging_bitmap_pdpt;

pdpt64_t  paging_tmap_pdpt;

pdpt64_t  paging_talloc_pdpt;

pdpt64_t  paging_valloc_pdpt;

pt64_t    paging_temp_pt;
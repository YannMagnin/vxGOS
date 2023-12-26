//---
//  vhex:mmu:mmu - MMU driver definition and context management
//---
#include <vhex/driver/mmu.h>
#include <vhex/driver/mpu/sh/sh7305/mmu.h>
#include <vhex/defs/attributes.h>

/* utlb_addr() - get the P4 address of a UTLB address entry */
VINLINE const struct utlb_addr *utlb_addr(uint E)
{
    uintptr_t addr = 0xf6000000 | ((E & 0x3f) << 8);
    return (void *)addr;
}

/* utlb_data() - get the P4 address of a UTLB data entry */
VINLINE const struct utlb_data *utlb_data(uint E)
{
    uintptr_t addr = 0xf7000000 | ((E & 0x3f) << 8);
    return (void *)addr;
}

/* mmu_translate(): Get the physical address for a virtual page */
uintptr_t mmu_translate(uintptr_t page, size_t *size)
{
    for(int E = 0; E < 64; E++)
    {
        const struct utlb_addr *addr = utlb_addr(E);
        const struct utlb_data *data = utlb_data(E);
        if(!addr->V || !data->V) continue;

        if((uintptr_t)addr->VPN << 10 == page)
        {
            /* Magic formula to get the size without using an array
               since this code is used even before global data is
               posibly initialized */
            int sz = ((data->SZ1 << 1) | data->SZ2) << 3;
            if(size) *size = 1 << ((0x14100c0a >> sz) & 0xff);

            return data->PPN << 10;
        }
    }
    return -1;
}

/* mmu_uram_size(): Get size of user RAM area */
size_t mmu_uram_size(void)
{
    size_t size = 0;
    size_t pagesize;

    while(mmu_translate(0x08100000 + size, &pagesize) != (uintptr_t)-1) {
        size += pagesize;
    }

    return size;
}

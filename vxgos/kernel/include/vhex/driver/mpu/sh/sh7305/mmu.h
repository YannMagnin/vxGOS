//---
//  vhex:mpu:mmu - Memory Management Unit
//
//  The MMU mainly exposes the contents of the TLB for us to inspect.
//  Functions to manipulate these are exposed by <vhex/mmu.h>.
//---


#ifndef __VHEX_MPU_SH7305_MMU__
# define __VHEX_MPU_SH7305_MMU__

#include <vhex/defs/attributes.h>

//---
//  SH7305 TLB. Refer to:
//    "Renesas SH7724 User's Manual: Hardware"
//    Section 7: "Memory Management Unit (MMU)"
//---

/* utlb_addr - address part of a UTLB entry */
struct utlb_addr
{
    uint VPN                :22;
    uint D                  :1;
    uint V                  :1;
    uint ASID               :8;

} VPACKED(4);

/* utlb_data - data part of a UTLB entry */
struct utlb_data
{
    uint                    :3;
    uint PPN                :19;
    uint                    :1;
    uint V                  :1;
    uint SZ1                :1;
    uint PR                 :2;
    uint SZ2                :1;
    uint C                  :1;
    uint D                  :1;
    uint SH                 :1;
    uint WT                 :1;

} VPACKED(4);

/* sh7305_mmu - MMU definition */
struct sh7305_mmu
{
    lword_union(PTEH,
        uint32_t VPN        :22;    /* Virtual Page Number */
        uint32_t            :2;
        uint32_t ASID       :8;     /* Address Space Identifier */
    );

    lword_union(PTEL,
        uint32_t            :3;
        uint32_t PPN        :19;    /* Phusical Page Number */
        uint32_t            :1;
        uint32_t V          :1;     /* Valid */
        uint32_t SZ1        :1;     /* Size (bit 1) */
        uint32_t PR         :2;     /* Protection */
        uint32_t SZ0        :1;     /* Size (bit 0) */
        uint32_t C          :1;     /* Cacheable */
        uint32_t D          :1;     /* Dirty */
        uint32_t SH         :1;     /* Shared */
        uint32_t WT         :1;     /* Write-through */
    );

    uint32_t TTB;
    uint32_t TEA;

    lword_union(MMUCR,
        uint32_t LRUI       :6;     /* Least-Recently Used ITLB */
        uint32_t            :2;
        uint32_t URB        :6;     /* UTLB Replace Boundary */
        uint32_t            :2;
        uint32_t URC        :6;     /* UTLB Replace Counter */
        uint32_t            :1;
        uint32_t SV         :1;     /* Single Virtual Memory Mode */
        uint32_t ME         :1;     /* TLB Extended Mode */
        uint32_t            :4;
        uint32_t TI         :1;     /* TLB Invalidate */
        uint32_t            :1;
        uint32_t AT         :1;     /* Address Translation */
    );
    pad(0x20);

    lword_union(PTEA,
        uint32_t            :18;
        uint32_t EPR        :6;
        uint32_t ESZ        :4;
        uint32_t            :4;
    );
    pad(0x38);

    lword_union(PASCR,
        uint32_t            :24;
        uint32_t UBC        :1;     /* Control register area */
        uint32_t UB6        :1;     /* Area 6 */
        uint32_t UB5        :1;     /* Area 5 */
        uint32_t UB4        :1;     /* Area 4 */
        uint32_t UB3        :1;     /* Area 3 */
        uint32_t UB2        :1;     /* Area 2 */
        uint32_t UB1        :1;     /* Area 1 */
        uint32_t UB0        :1;     /* Area 0 */
    );
    pad(4);

    lword_union(IRMCR,
        uint32_t            :27;
        uint32_t R2         :1;     /* Re-fetch after Register 2 change */
        uint32_t R1         :1;     /* Re-fetch after Register 1 change */
        uint32_t LT         :1;     /* Re-fetch after LDTLB */
        uint32_t MT         :1;     /* Re-fetch after writing TLB */
        uint32_t MC         :1;     /* Re-fetch after writing insn cache */
    );

} VPACKED(4);

#define SH7305_MMU (*(volatile struct sh7305_mmu *)0xff000000)

#endif /*__VHEX_MPU_SH7305_MMU__*/

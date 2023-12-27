#ifndef __VHEX_MPU_SH7305_CPG__
# define __VHEX_MPU_SH7305_CPG__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>


//---
//  SH7305 Clock Pulse Generator. Refer to:
//    "Renesas SH7724 User's Manual: Hardware"
//    Section 17: "Clock Pulse Generator (CPG)"
//---

/* sh7305_cpg - Clock Pulse Generator registers
   Fields marked with [*] don't have the meaning described in the SH7724
   documentation. */
struct __sh7305_cpg
{
    lword_union(FRQCR,
        uint32_t KICK       :1;     /* Flush FRQCRA modifications */
        uint32_t            :1;
        uint32_t STC        :6;     /* PLL multiplication [*] */
        uint32_t IFC        :4;     /* Iphi divider 1 [*] */
        uint32_t            :4;
        uint32_t SFC        :4;     /* Sphi divider 1 [*] */
        uint32_t BFC        :4;     /* Bphi divider 1 [*] */
        uint32_t            :4;
        uint32_t PFC        :4;     /* Pphi divider 1 [*] */
    );
    pad(0x4);

    lword_union(FSICLKCR,
        uint32_t            :16;
        uint32_t DIVB       :6;     /* Division ratio for port B */
        uint32_t            :1;
        uint32_t CLKSTP     :1;     /* Clock Stop */
        uint32_t SRC        :2;     /* Clock source select */
        uint32_t DIVA       :6;     /* Division ratio for port A */
    );
    pad(0x04);

    lword_union(DDCLKCR,
        uint32_t            :23;
        uint32_t CLKSTP     :1;     /* Clock Stop */
        uint32_t SRC        :1;     /* Clock source select */
        uint32_t            :1;
        uint32_t DIV        :6;
    );

    lword_union(USBCLKCR,
        uint32_t            :23;
        uint32_t CLKSTP     :1;     /* Clock Stop */
        uint32_t            :8;
    );
    pad(0x0c);

    lword_union(PLLCR,
        uint32_t            :17;
        uint32_t PLLE       :1;     /* PLL Enable */
        uint32_t            :1;
        uint32_t FLLE       :1;     /* FLL Enable */
        uint32_t            :10;
        uint32_t CKOFF      :1;     /* CKO Output Stop */
        uint32_t            :1;
    );

    lword_union(PLL2CR,
        uint32_t KICK       :1;     /* Flush ??? modification */
        uint32_t            :1;
        uint32_t MUL        :6;     /* ??? multiplication ration */
        uint32_t            :24;
    );
    pad(0x10);

    lword_union(SPUCLKCR,
        uint32_t            :23;
        uint32_t CLKSTP     :1;     /* Clock Stop */
        uint32_t SRC        :1;     /* Clock source select */
        uint32_t            :1;
        uint32_t DIV        :6;     /* Division ratio */
    );
    pad(0x4);

    lword_union(SSCGCR,
        uint32_t SSEN       :1;     /* Spread Spectrum Enable */
        uint32_t            :30;
        uint32_t SSEN_EMU   :1;     /* Emulateur enable */
    );
    pad(0x8);

    lword_union(FLLFRQ,
        uint32_t            :16;
        uint32_t SELXM      :2;     /* FLL output division */
        uint32_t            :3;
        uint32_t FLF        :11;    /* FLL Multiplication Ratio */
    );
    pad(0x0c);

    lword_union(LSTATS,
        uint32_t            :31;
        uint32_t FRQF       :1;    /* frequency changing status */
    );

} VPACKED(4);

#define SH7305_CPG (*((volatile struct __sh7305_cpg *)0xa4150000))

//---
// kernel-level API
//---

/* cpg_clock_frequency
   A dump of the Clock Pulse Generator's (CPG) configuration.*/
struct cpg_clock_frequency {
    int PLL;
    int FLL;
    int base;

    int Bphi_div;
    int Iphi_div;
    int Sphi_div;
    int Pphi_div;

    int RTCCLK_f;

    int Bphi_f;
    int Iphi_f;
    int Sphi_f;
    int Pphi_f;

};

/* cpg_clock_freq() - get the frequency of the main clocks
   This function returns the address of a static object which is used by the
   module; this address never changes. */
extern void cpg_clock_freq(struct cpg_clock_frequency *freq);



#endif /* __VHEX_MPU_SH7305_CPG__ */

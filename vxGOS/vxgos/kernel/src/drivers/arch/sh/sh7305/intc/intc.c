#include <vhex/driver.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>

#include <string.h>

/* Construct the Interrupt Controller */
struct sh7305_intc SH7305_INTC = {
    .ICR0           = (void *)0xa4140000,
    .ICR1           = (void *)0xa414001c,
    .INTPRI00       = (void *)0xa4140010,
    .INTREQ00       = (void *)0xa4140024,
    .INTMSK00       = (void *)0xa4140044,
    .INTMSKCLR00    = (void *)0xa4140064,
    .NMIFCR         = (void *)0xa41400c0,

    .IPR            = (void *)0xa4080000,
    .MSK            = (void *)0xa4080080,
    .MSKCLR         = (void *)0xa40800c0,
    .USERIMASK      = (void *)0xa4700000,

//  PINTCR      = (void *)0xa40501dc,
//  PINTSR      = (void *)0xa40501ea,
};


/* define the private INTerrupt Controller context structure */
struct intc_ctx {
    uint16_t ipr[12];
    uint8_t  imr[13];
    sh7305_intc_userimask_t userimask;
    sh7305_intc_icr0_t icr0;
    sh7305_intc_icr1_t icr1;
    sh7305_intc_intpri00_t intpri00;
    sh7305_intc_intreq00_t intreq00;
    sh7305_intc_intmsk00_t intmsk00;
    sh7305_intc_nmifcr_t nmifcr;
};

//---
// hardware-level API
//---

/* Interrupt IPR and IMR positions. */

/* Friendly names for IPR and IMR register numbers */
enum{ IPRA, IPRB, IPRC, IPRD, IPRE, IPRF, IPRG, IPRH, IPRI, IPRJ, IPRK, IPRL };
enum{ IMR0, IMR1, IMR2, IMR3, IMR4, IMR5, IMR6, IMR7, IMR8, IMR9, IMR10 };
#define _ 0,0

static struct info {
    uint16_t IPR4, IPR4bits, IMR, IMRbits;
} const info[] = {
    /* Standard TMU */
    { IPRA, 0xf000, IMR4,  0x10 },
    { IPRA, 0x0f00, IMR4,  0x20 },
    { IPRA, 0x00f0, IMR4,  0x40 },
    /* ETMU */
    { IPRJ, 0xf000, IMR6,  0x08 },
    { IPRG, 0x0f00, IMR5,  0x02 },
    { IPRG, 0x00f0, IMR5,  0x04 },
    { IPRE, 0x00f0, IMR2,  0x01 },
    { IPRI, 0xf000, IMR6,  0x10 },
    { IPRL, 0xf000, IMR8,  0x02 },
    /* DMA */
    { IPRE, 0xf000, IMR1,  0x01 },
    { IPRE, 0xf000, IMR1,  0x02 },
    { IPRE, 0xf000, IMR1,  0x04 },
    { IPRE, 0xf000, IMR1,  0x08 },
    { IPRF, 0x0f00, IMR5,  0x10 },
    { IPRF, 0x0f00, IMR5,  0x20 },
    { IPRF, 0x0f00, IMR5,  0x40 },
    /* RTC */
    { IPRK, 0xf000, IMR10, 0x04 },
    { IPRK, 0xf000, IMR10, 0x02 },
    { IPRK, 0xf000, IMR10, 0x01 },
    /* SPU */
    { IPRC, 0x000f, IMR3,  0x04 },
    { IPRC, 0x000f, IMR4,  0x08 },
    /* USB */
    { IPRF, 0x00f0, IMR9,  0x02 },
    /* KEYSC */
    { IPRF, 0xf000, IMR5,  0x80 }
};

/* sh7305_intc_priority() : set the interruption level */
int sh7305_intc_priority(int intname, int level)
{
    struct info const *i = &info[intname];
    int IPRn = i->IPR4, IPRbits = i->IPR4bits;

    /* Bit-shift for the mask */
    int shift = 0;
    while(IPRbits >>= 4) shift += 4;

    uint16_t volatile *IPR = &SH7305_INTC.IPR[IPRn << 1];

    int oldlevel = (*IPR >> shift) & 0xf;
    *IPR = (*IPR & ~(0xf << shift)) | (level << shift);

    if(level > 0) {
        uint8_t volatile *MSKCLR = SH7305_INTC.MSKCLR;
        MSKCLR[4*i->IMR] = i->IMRbits;
    } else {
        uint8_t volatile *MSK = SH7305_INTC.MSK;
        MSK[4*i->IMR] = i->IMRbits;
    }

    return oldlevel;
}

/* sh7305_intc_install_inth() : install interrupt gate */
void *sh7305_intc_install_inth_gate(int event_code, void *gate, size_t size)
{
    extern uintptr_t vhex_vbr;

    return memcpy(
        (void*)((uintptr_t)&vhex_vbr + 0x600 + 0x40 + (event_code - 0x400)),
        gate,
        size
    );
}

/* sh7305_intc_generic_handler() : install generic interrupt handler */
void *sh7305_intc_install_inth_generic(int event_code, vhex_call_t callback)
{
    extern void sh7305_intc_inth_generic_gate(void);
    extern void sh7305_inth_callback(void);

    uint8_t *h = sh7305_intc_install_inth_gate(
        event_code,
        &sh7305_intc_inth_generic_gate,
        32
    );
    if(h != NULL) {
        uintptr_t workaround = (uintptr_t)&sh7305_inth_callback;
        memcpy(&h[8],  &callback, 20);
        memcpy(&h[28], &workaround, 4);
    }
    return h;
}


//---
// hardware configuration call
//---

#if 0
static bool __intc_hpowered(void)
{
    return (SH7305_POWER.MSTPCR0.INTC == 0);
}

static int __intc_hpoweron(void)
{
    SH7305_POWER.MSTPCR0.INTC = 0;
    return 0;
}

static int __intc_hpoweroff(void)
{
    SH7305_POWER.MSTPCR0.INTC = 1;
    return 0;
}
#endif

/* __intc_configure() : configure the Interrupt Controller

      Note that we don't touch to the NMI configuration because this part is
   obscure. I have done some reverse-ingenering in this part, but Casio seems
   use this part oddly. So, to avoid any probleme with Non-Maskable Interrupt,
   we will use the Casio's configuration.

      So, by default, mask all interrupt and enable all interrupt is the user
   interrupt mask */
static void __intc_configure(struct intc_ctx *state)
{
    memset(state, 0x00, sizeof(struct intc_ctx));

    for (int i = 0; i < 12; ++i)
        state->ipr[i] = 0x0000;

    for (int i = 0; i < 13; ++i)
        state->imr[i] = 0x00;

    state->userimask._0xa5  = 0xa5;
    state->userimask.UIMASK = 0x00;

    /* interrupt control register 0 (NMI)
         <> enable NMI interruption when a input level is low
         <> keep NMI interrupt pending while the SR.BL=1
         <> interrupt request is detected on falling edge */
    state->icr0.MAI   = 0;
    state->icr0.NMIB  = 0;
    state->icr0.NMIE  = 0;
    state->icr0._HIGH = 0b11;

    /* Interrupt control register 1 (IRQ)
         <> interrupt request is detected on low-level of IRQx input */
    state->icr1.IRQ0S = 0b10;
    state->icr1.IRQ1S = 0b10;
    state->icr1.IRQ2S = 0b10;
    state->icr1.IRQ3S = 0b10;

    /* Interrupt priority (IRQ)
         <> mask all IRQx interrupt */
    state->intpri00.IRQ0 = 0b0000;
    state->intpri00.IRQ1 = 0b0000;
    state->intpri00.IRQ2 = 0b0000;
    state->intpri00.IRQ3 = 0b0000;

    /* Interrupt request (IRQ)
         <> clear all pending interrupt request */
    state->intreq00.IRQ0 = 0;
    state->intreq00.IRQ1 = 0;
    state->intreq00.IRQ2 = 0;
    state->intreq00.IRQ3 = 0;

    /* Interurpt mask (IRQ)
         <> mask all IRQ interrupt */
    state->intmsk00.IRQ0 = 1;
    state->intmsk00.IRQ1 = 1;
    state->intmsk00.IRQ2 = 1;
    state->intmsk00.IRQ3 = 1;

    /* NMI Interrupt flags (clear) */
    state->nmifcr.NMIFL = 0;
}


/* hardware hypervisor call */

/* __intc_hsave() : save hardware information */
static void __intc_hsave(struct intc_ctx *state)
{
    for (int i = 0; i < 12; ++i)
        state->ipr[i] = SH7305_INTC.IPR[i << 1];

    for (int i = 0; i < 13; ++i)
        state->imr[i] = SH7305_INTC.MSK[i << 2];

    state->userimask = *SH7305_INTC.USERIMASK;

    state->icr0 = *SH7305_INTC.ICR0;
    state->icr1 = *SH7305_INTC.ICR1;
    state->intpri00 = *SH7305_INTC.INTPRI00;
    state->intreq00 = *SH7305_INTC.INTREQ00;
    state->intmsk00 = *SH7305_INTC.INTMSK00;
}

/* __intc_hrestore() : restore hardware information */
static void __intc_hrestore(struct intc_ctx *state)
{
    for (int i = 0; i < 12; ++i)
        SH7305_INTC.IPR[i << 1] = state->ipr[i];

    for (int i = 0; i < 13; ++i) {
        SH7305_INTC.MSKCLR[i << 2] = 0xff;
        SH7305_INTC.MSK[i << 2] = state->imr[i];
    }

    state->userimask._0xa5  = 0xa5;
    *SH7305_INTC.USERIMASK  = state->userimask;

    *SH7305_INTC.ICR0 = state->icr0;
    *SH7305_INTC.ICR1 = state->icr1;
    *SH7305_INTC.INTPRI00 = state->intpri00;
    *SH7305_INTC.INTREQ00 = state->intreq00;
    *SH7305_INTC.INTMSKCLR00 = (sh7305_intc_intmskclr00_t){
        .IRQ0 = 1,
        .IRQ1 = 1,
        .IRQ2 = 1,
        .IRQ3 = 1,
    };
    *SH7305_INTC.INTMSK00 = state->intmsk00;
}

/* declare the INTC driver */

struct vhex_driver drv_intc = {
    .name       = "INTC",
    .hsave      = (void*)&__intc_hsave,
    .hrestore   = (void*)&__intc_hrestore,
    .configure  = (void*)&__intc_configure,
    .state_size = sizeof(struct intc_ctx)
};
VHEX_DECLARE_DRIVER(01, drv_intc);

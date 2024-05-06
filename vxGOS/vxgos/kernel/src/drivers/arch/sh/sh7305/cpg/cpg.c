#include <vhex/driver/mpu/sh/sh7305/cpg.h>
#include <vhex/driver.h>

#define CPG SH7305_CPG

void cpg_clock_freq(struct cpg_clock_frequency *freq)
{
    /* The meaning of the PLL setting on SH7305 differs from the
       documentation of SH7224; the value must not be doubled. */
    int pll = CPG.FRQCR.STC + 1;
    freq->PLL = pll;

    /* The FLL ratio is the value of the setting, halved if SELXM=1 */
    int fll = CPG.FLLFRQ.FLF;
    if(CPG.FLLFRQ.SELXM == 1) fll >>= 1;
    freq->FLL = fll;

    /* On SH7724, the divider ratio is given by 1 / (setting + 1), but on
       the SH7305 it is 1 / (2^setting + 1). */

    int divb = CPG.FRQCR.BFC;
    int divi = CPG.FRQCR.IFC;
    int divs = CPG.FRQCR.SFC;
    int divp = CPG.FRQCR.PFC;

    freq->Bphi_div = 1 << (divb + 1);
    freq->Iphi_div = 1 << (divi + 1);
    freq->Sphi_div = 1 << (divs + 1);
    freq->Pphi_div = 1 << (divp + 1);

    /* Deduce the input frequency of divider 1 */
    freq->base = 32768;
    if(CPG.PLLCR.FLLE) freq->base *= fll;
    if(CPG.PLLCR.PLLE) freq->base *= pll;

    /* And the frequency of all other input clocks */
    freq->RTCCLK_f = 32768;
    freq->Bphi_f   = freq->base >> (divb + 1);
    freq->Iphi_f   = freq->base >> (divi + 1);
    freq->Sphi_f   = freq->base >> (divs + 1);
    freq->Pphi_f   = freq->base >> (divp + 1);
}


//---
// Define driver information
//---

struct cpg_ctx {
    struct __sh7305_cpg hw;
};

/* __cpg_configure() : configure the CPG */
static void __cpg_configure(struct cpg_ctx *state)
{
    /* Configure the all bus divier and multiplicator
       <> PLL               : 232.31Mhz (x16)
       <> Iphi (CPU)        : 116.15Mhz (/2)
       <> Sphi (SuperH)     :  58.07Mhz (/4)
       <> Bphi (BSC)        :  58.07Mhz (/4)
       <> Pphi (peripheral) :  29.03Mhz (/8)
    */
    state->hw.FRQCR.STC  = 15;
    state->hw.FRQCR.IFC  = 0;
    state->hw.FRQCR.SFC  = 1;
    state->hw.FRQCR.BFC  = 1;
    state->hw.FRQCR.PFC  = 2;

    /* Configure FSI clock (disable by default) */
    state->hw.FSICLKCR.CLKSTP = 1;

    /* Configure DD clock (disable by default) */
    state->hw.DDCLKCR.CLKSTP  = 1;

    /* Configure USB clock (disable by default) */
    state->hw.USBCLKCR.CLKSTP = 1;

    /* Configure PLL and FLL multiplier
       <> enable PLL
       <> enable FLL
       <> disable CKO output stop
    */
    state->hw.PLLCR.PLLE  = 1;
    state->hw.PLLCR.FLLE  = 1;
    state->hw.PLLCR.CKOFF = 0;

    /* Configure PLL multiplication ratio (disable by default) */
    state->hw.PLL2CR.MUL  = 0;

    /* Configure SPU (disable by default) */
    state->hw.SPUCLKCR.CLKSTP = 1;

    /* Configure Spread Spectrum (force disable) */
    state->hw.SSCGCR.SSEN_EMU = 0;
    state->hw.SSCGCR.SSEN = 0;

    /* Configure FLL division/multiplicator
       <> force FLL multiplication Ratio with x(1/2)
       <> use 1017 (default value of the module) as multiplicator wich will
           give us ~16Mhz (Casio use 900 instead of 1017 wich is ~14Mhz):

                ( 32768 * 1017 ) / 2 = 16 662 528 Hz
                ( 32768 * 900  ) / 2 = 14 745 600 Hz
    */
    state->hw.FLLFRQ.SELXM = 1;
    state->hw.FLLFRQ.FLF = 1017;
}

/* __cpg_hsave() : save hardware information */
static void __cpg_hsave(struct cpg_ctx *state)
{
    state->hw.FRQCR.STC = SH7305_CPG.FRQCR.STC;
    state->hw.FRQCR.IFC = SH7305_CPG.FRQCR.IFC;
    state->hw.FRQCR.SFC = SH7305_CPG.FRQCR.SFC;
    state->hw.FRQCR.BFC = SH7305_CPG.FRQCR.BFC;
    state->hw.FRQCR.PFC = SH7305_CPG.FRQCR.PFC;

    state->hw.FSICLKCR.DIVB   = SH7305_CPG.FSICLKCR.DIVB;
    state->hw.FSICLKCR.CLKSTP = SH7305_CPG.FSICLKCR.CLKSTP;
    state->hw.FSICLKCR.SRC    = SH7305_CPG.FSICLKCR.SRC;
    state->hw.FSICLKCR.DIVA   = SH7305_CPG.FSICLKCR.DIVA;

    state->hw.DDCLKCR.CLKSTP = SH7305_CPG.DDCLKCR.CLKSTP;
    state->hw.DDCLKCR.SRC    = SH7305_CPG.DDCLKCR.SRC;
    state->hw.DDCLKCR.DIV    = SH7305_CPG.DDCLKCR.DIV;

    state->hw.USBCLKCR.CLKSTP = SH7305_CPG.USBCLKCR.CLKSTP;

    state->hw.PLLCR.PLLE  = SH7305_CPG.PLLCR.PLLE;
    state->hw.PLLCR.FLLE  = SH7305_CPG.PLLCR.FLLE;
    state->hw.PLLCR.CKOFF = SH7305_CPG.PLLCR.CKOFF;

    state->hw.PLL2CR.MUL = SH7305_CPG.PLL2CR.MUL;

    state->hw.SPUCLKCR.CLKSTP = SH7305_CPG.SPUCLKCR.CLKSTP;
    state->hw.SPUCLKCR.SRC    = SH7305_CPG.SPUCLKCR.SRC;
    state->hw.SPUCLKCR.DIV    = SH7305_CPG.SPUCLKCR.DIV;

    state->hw.SSCGCR.SSEN     = SH7305_CPG.SSCGCR.SSEN;
    state->hw.SSCGCR.SSEN_EMU = SH7305_CPG.SSCGCR.SSEN_EMU;

    state->hw.FLLFRQ.SELXM  = SH7305_CPG.FLLFRQ.SELXM;
    state->hw.FLLFRQ.FLF    = SH7305_CPG.FLLFRQ.FLF;
}

/* __cpg_hrestore() : restore hardware information */
static void __cpg_hrestore(struct cpg_ctx *state)
{
    SH7305_CPG.FRQCR.STC       = state->hw.FRQCR.STC;
    SH7305_CPG.FRQCR.IFC       = state->hw.FRQCR.IFC;
    SH7305_CPG.FRQCR.SFC       = state->hw.FRQCR.SFC;
    SH7305_CPG.FRQCR.BFC       = state->hw.FRQCR.BFC;
    SH7305_CPG.FRQCR.PFC       = state->hw.FRQCR.PFC;

    SH7305_CPG.FSICLKCR.DIVB   = state->hw.FSICLKCR.DIVB;
    SH7305_CPG.FSICLKCR.CLKSTP = state->hw.FSICLKCR.CLKSTP;
    SH7305_CPG.FSICLKCR.SRC    = state->hw.FSICLKCR.SRC;
    SH7305_CPG.FSICLKCR.DIVA   = state->hw.FSICLKCR.DIVA;

    SH7305_CPG.DDCLKCR.CLKSTP  = state->hw.DDCLKCR.CLKSTP;
    SH7305_CPG.DDCLKCR.SRC     = state->hw.DDCLKCR.SRC;
    SH7305_CPG.DDCLKCR.DIV     = state->hw.DDCLKCR.DIV;

    SH7305_CPG.USBCLKCR.CLKSTP = state->hw.USBCLKCR.CLKSTP;

    SH7305_CPG.PLLCR.PLLE      = state->hw.PLLCR.PLLE;
    SH7305_CPG.PLLCR.FLLE      = state->hw.PLLCR.FLLE;
    SH7305_CPG.PLLCR.CKOFF     = state->hw.PLLCR.CKOFF;

    SH7305_CPG.PLL2CR.MUL      = state->hw.PLL2CR.MUL;

    SH7305_CPG.SPUCLKCR.CLKSTP = state->hw.SPUCLKCR.CLKSTP;
    SH7305_CPG.SPUCLKCR.SRC    = state->hw.SPUCLKCR.SRC;
    SH7305_CPG.SPUCLKCR.DIV    = state->hw.SPUCLKCR.DIV;

    SH7305_CPG.SSCGCR.SSEN     = state->hw.SSCGCR.SSEN;
    SH7305_CPG.SSCGCR.SSEN_EMU = state->hw.SSCGCR.SSEN_EMU;

    SH7305_CPG.FLLFRQ.SELXM    = state->hw.FLLFRQ.SELXM;
    SH7305_CPG.FLLFRQ.FLF      = state->hw.FLLFRQ.FLF;

    SH7305_CPG.FRQCR.KICK = 1;
    while (SH7305_CPG.LSTATS.FRQF != 0) { __asm__ volatile ("nop"); }

    SH7305_CPG.PLL2CR.KICK = 1;
    while (SH7305_CPG.LSTATS.FRQF != 0) { __asm__ volatile ("nop"); }
}

struct vhex_driver drv_cpg = {
    .name       = "CPG",
    .hsave      = (void*)&__cpg_hsave,
    .hrestore   = (void*)&__cpg_hrestore,
    .configure  = (void*)&__cpg_configure,
    .state_size = sizeof(struct cpg_ctx)
};
VHEX_DECLARE_DRIVER(03, drv_cpg);

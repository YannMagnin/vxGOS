#include <vhex/driver/mpu/sh/sh7305/keysc.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>
#include <vhex/keyboard/interface.h>
#include <vhex/driver.h>

/* define the private KeyScan context structure */
struct keysc_ctx {
    struct __sh7305_keysc_s keysc;
};

/* hardware configuration call */

static bool __keysc_hpowered(void)
{
    return (SH7305_POWER.MSTPCR0.KEYSC == 0);
}

static int __keysc_hpoweron(void)
{
    SH7305_POWER.MSTPCR0.KEYSC = 0;
    return 0;
}

static int __keysc_hpoweroff(void)
{
    SH7305_POWER.MSTPCR0.KEYSC = 1;
    return 0;
}

/* __keysc_configure() : configure the SH7305_KEYSC module */
static void __keysc_configure(struct keysc_ctx *ctx)
{
    /* configure the scan control
       <> enable the SH7305_KEYSC interruption (0xbe0)
       <> set undocumented bit _O to 0 (Casio's configuration)
       <> set undocumented bit _1 to 0 (Casio's configuration) */
    ctx->keysc.KIUCNTREG.KEYIE = 1;
    ctx->keysc.KIUCNTREG._0 = 0;
    ctx->keysc.KIUCNTREG._1 = 0;

    /* configure the key-bounce fix setting
       <> enable the key-bounce "fix"
       <> set the undocumented bit _0 to 0b100 (Casio's configuration)
       <> set the undocumented bit _1 to 0b010 (Casio's configuration) */
    ctx->keysc.KIAUTOFIXREG.KEYBE = 1;
    ctx->keysc.KIAUTOFIXREG._0 = 0b100;
    ctx->keysc.KIAUTOFIXREG._1 = 0b010;

    /* configure the scan mode
       <> set undocumented bit _O to 0 (Casio's configuration)
       <> set undocumented bit _1 to 0 (Casio's configuration)
       <> set undocumented bit _2 to 1 (Casio's configuration) */
    ctx->keysc.KIUMODEREG._0 = 0;
    ctx->keysc.KIUMODEREG._1 = 0;
    ctx->keysc.KIUMODEREG._2 = 1;

    /* configure the interrupt setting
       This part is wierd because I cannot explain why this configuration
       generate the whanted behaviour (generate interrupt when a falling
       or a pressed or a rising edge is detected on any keys).

       This configuration has been discovered by practicing a lot a blind
       test with the hardware. */
    ctx->keysc.KIUINTREG.KYCPU_IE = 0b0000010;

    /* configure the time between each interrupt
       <> remove time between interruption (Casio's configuration) */
    ctx->keysc.KIUWSETREG.TIME = 0x05;

    /* configure the interval value between each scan
       <> use the Casio's configuration */
    ctx->keysc.KIUINTERVALREG = 0x0098;

    /* configure the key output ???? */
    ctx->keysc.KYOUTDR.KYO5DT = 0b11;
    ctx->keysc.KYOUTDR.KYO4DT = 0b11;
    ctx->keysc.KYOUTDR.KYO3DT = 0b11;
    ctx->keysc.KYOUTDR.KYO2DT = 0b11;
    ctx->keysc.KYOUTDR.KYO1DT = 0b11;
    ctx->keysc.KYOUTDR.KYO0DT = 0b11;

    /* configure the key input setting
       <> enable F1, SHIFT, ALPHA, xoT, div, 7, 4, 1, 0
       <> enable F2, OPT, sqrt, log, FD, 8, 5, 2, .
       <> enable F3, var, ^, ln, (, 9, 6, 3, expo
       <> enable F4, menu, exit, sin, ), del, x, +, (-)
       <> enable F5, <, down, cos, [,], %, -, EXE
       <> enable F6, up, >, tan, ->
       <> enable AC/ON */
    ctx->keysc.KYINDR.KYDIR6 = 1;
    ctx->keysc.KYINDR.KYDIR5 = 1;
    ctx->keysc.KYINDR.KYDIR4 = 1;
    ctx->keysc.KYINDR.KYDIR3 = 1;
    ctx->keysc.KYINDR.KYDIR2 = 1;
    ctx->keysc.KYINDR.KYDIR1 = 1;
    ctx->keysc.KYINDR.KYDIR0 = 1;

    /* set the interrupt level (max) */
    sh7305_intc_priority(INTC_KEYSC, 15);

    /* intall the KEYSC interupt handler */
    extern void sh7305_keysc_inth(void);
    sh7305_intc_install_inth_gate(0xbe0, &sh7305_keysc_inth, 64);
}

/* __keysc_hsave() : save hardware information */
static void __keysc_hsave(struct keysc_ctx *ctx)
{
    ctx->keysc.KIUCNTREG.word    = SH7305_KEYSC.KIUCNTREG.word;
    ctx->keysc.KIAUTOFIXREG.word = SH7305_KEYSC.KIAUTOFIXREG.word;
    ctx->keysc.KIUMODEREG.word   = SH7305_KEYSC.KIUMODEREG.word;
    ctx->keysc.KIUINTREG.word    = SH7305_KEYSC.KIUINTREG.word;
    ctx->keysc.KIUWSETREG.word   = SH7305_KEYSC.KIUWSETREG.word;
    ctx->keysc.KIUINTERVALREG    = SH7305_KEYSC.KIUINTERVALREG;
    ctx->keysc.KYOUTDR.word      = SH7305_KEYSC.KYOUTDR.word;
    ctx->keysc.KYINDR.word       = SH7305_KEYSC.KYINDR.word;
}

/* __keysc_hrestore() : restore hadware information */
static void __keysc_hrestore(struct keysc_ctx *ctx)
{
    /* force mask the SH7305_KEYSC module, otherwise the module will crash */
    SH7305_INTC._MSK->IMR5.KEYSC = 1;

    /* restore registers */
    SH7305_KEYSC.KIUCNTREG.word    = ctx->keysc.KIUCNTREG.word;
    SH7305_KEYSC.KIAUTOFIXREG.word = ctx->keysc.KIAUTOFIXREG.word;
    SH7305_KEYSC.KIUMODEREG.word   = ctx->keysc.KIUMODEREG.word;
    SH7305_KEYSC.KIUINTREG.word    = ctx->keysc.KIUINTREG.word;
    SH7305_KEYSC.KIUWSETREG.word   = ctx->keysc.KIUWSETREG.word;
    SH7305_KEYSC.KIUINTERVALREG    = ctx->keysc.KIUINTERVALREG;
    SH7305_KEYSC.KYOUTDR.word      = ctx->keysc.KYOUTDR.word;
    SH7305_KEYSC.KYINDR.word       = ctx->keysc.KYINDR.word;

    /* enable keysc interuption */
    SH7305_INTC._MSKCLR->IMR5.KEYSC = 1;
}

/* declare the KEYSC driver */

struct vhex_driver drv_keysc = {
    .name       = "KEYSC",
    .hpowered   = (void*)&__keysc_hpowered,
    .hpoweron   = (void*)&__keysc_hpoweron,
    .hpoweroff  = (void*)&__keysc_hpoweroff,
    .hsave      = (void*)&__keysc_hsave,
    .hrestore   = (void*)&__keysc_hrestore,
    .configure  = (void*)&__keysc_configure,
    .state_size = sizeof(struct keysc_ctx),
    .flags = {
        .KEYBOARD = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct keyboard_drv_interface){
        .keycache_init       = &sh7305_keycache_init,
        .keycache_keydown    = &sh7305_keycache_keydown,
        .keycache_event_wait = &sh7305_keycache_event_wait,
        .keycache_event_pop  = &sh7305_keycache_event_pop,
        .keycache_event_push = &sh7305_keycache_event_push,
        .keycache_quit       = &sh7305_keycache_quit,
    }
};
VHEX_DECLARE_DRIVER(03, drv_keysc);

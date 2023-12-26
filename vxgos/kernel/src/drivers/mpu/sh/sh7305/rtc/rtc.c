#include <vhex/driver/mpu/sh/sh7305/rtc.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>
#include <vhex/rtc/interface.h>
#include <vhex/defs/call.h>
#include <vhex/driver.h>

#include <string.h>

//---
// RTC periodic interrupt
//---

static vhex_call_t rtc_call;

/* rtc_periodic interrupt() : internal interrupt handler */
static void sh7305_rtc_periodic_inth(void)
{
    int rc = vhex_call(rtc_call);

    /* Clear the interrupt flag */
    do SH7305_RTC.RCR2.PEF = 0;
    while(SH7305_RTC.RCR2.PEF);

    /* Stop the interrupt if the callback returns non-zero */
    if(rc) sh7305_rtc_periodic_disable();
}

/* rtc_periodic_enable(): Enable the periodic interrupt */
bool sh7305_rtc_periodic_enable(int frequency, vhex_call_t callback)
{
    /* Refuse to override an existing interrupt */
    if(SH7305_RTC.RCR2.PES != RTC_NONE) return false;
    if(frequency == RTC_NONE)
    {
        sh7305_rtc_periodic_disable();
        return true;
    }

    /* Temporarily disable the interrupt and set up the callback */
    SH7305_RTC.RCR2.PES = RTC_NONE;
    memcpy(&rtc_call, &callback, sizeof(vhex_call_t));

    /* Clear the interrupt flag */
    do SH7305_RTC.RCR2.PEF = 0;
    while(SH7305_RTC.RCR2.PEF);

    /* Enable the interrupt */
    SH7305_RTC.RCR2.PES = frequency;
    return true;
}

/* rtc_periodic_disable(): Stop the periodic interrupt */
void sh7305_rtc_periodic_disable(void)
{
    SH7305_RTC.RCR2.PES = RTC_NONE;
}



//---
// User API
//---

/* int8(), int16(): Convert BCD to integer */
static int int8(uint8_t bcd)
{
    return (bcd & 0x0f) + 10 * (bcd >> 4);
}
static int int16(uint16_t bcd)
{
    return (bcd & 0xf) + 10 * ((bcd >> 4) & 0xf) + 100 * ((bcd >> 8) & 0xf)
        + 1000 * (bcd >> 12);
}

/* bcd8(), bcd16(): Convert integer to BCD */
static uint8_t bcd8(int integer)
{
    integer %= 100;
    return ((integer / 10) << 4) | (integer % 10);
}
static uint16_t bcd16(int integer)
{
    integer %= 10000;
    return (bcd8(integer / 100) << 8) | bcd8(integer % 100);
}

/* sh7305_rtc_get_time(): Read the current time from the RTC */
int sh7305_rtc_get_time(rtc_time_t *time)
{
    do {
        SH7305_RTC.RCR1.CF = 0;

        time->ticks     = SH7305_RTC.R64CNT;
        time->seconds   = int8(SH7305_RTC.RSECCNT.byte);
        time->minutes   = int8(SH7305_RTC.RMINCNT.byte);
        time->hours     = int8(SH7305_RTC.RHRCNT.byte);
        time->month_day = int8(SH7305_RTC.RDAYCNT.byte);
        time->month     = int8(SH7305_RTC.RMONCNT.byte);
        time->year      = int16(SH7305_RTC.RYRCNT.word);
        time->week_day  = SH7305_RTC.RWKCNT;

    } while(SH7305_RTC.RCR1.CF != 0);
    return 0;
}

/* sh7305_rtc_set_time(): Set current time in the RTC */
int sh7305_rtc_set_time(rtc_time_t const *time)
{
    int wday = time->week_day;
    if(wday >= 7) wday = 0;

    do {
        SH7305_RTC.RCR1.CF = 0;

        SH7305_RTC.RSECCNT.byte = bcd8(time->seconds);
        SH7305_RTC.RMINCNT.byte = bcd8(time->minutes);
        SH7305_RTC.RHRCNT.byte  = bcd8(time->hours);
        SH7305_RTC.RDAYCNT.byte = bcd8(time->month_day);
        SH7305_RTC.RMONCNT.byte = bcd8(time->month);
        SH7305_RTC.RYRCNT.word  = bcd16(time->year);
        SH7305_RTC.RWKCNT       = wday;

    } while(SH7305_RTC.RCR1.CF != 0);
    return 0;
}

//---
// Define driver information
//---

struct rtc_ctx {
    struct __sh7305_rtc_s rtc;
};

static bool __rtc_hpowered(void)
{
    return (SH7305_POWER.MSTPCR0.RTC == 0);
}

static int __rtc_hpoweron(void)
{
    SH7305_POWER.MSTPCR0.RTC = 0;
    return 0;
}

static int __rtc_hpoweroff(void)
{
    SH7305_POWER.MSTPCR0.RTC = 1;
    return 0;
}

/* __rtc_configure() : configure the RTC */
static void __rtc_configure(struct rtc_ctx *state)
{
    /* Clear and disable the carry and alamr interrupts */
    state->rtc.RCR1.CF    = 0;
    state->rtc.RCR1.CIE   = 0;
    state->rtc.RCR1.AIE   = 0;
    state->rtc.RCR1.AF    = 0;

    /* Disable the periodic interrupt flag */
    state->rtc.RCR2.PEF   = 0;
    state->rtc.RCR2.PES   = RTC_NONE;
    state->rtc.RCR2.ADJ   = 0;
    state->rtc.RCR2.RESET = 1;
    state->rtc.RCR2.START = 1;

    /* Disable year alarm check */
    state->rtc.RCR3.BEN   = 0;

    /* set the interrupt level (max) */
    sh7305_intc_priority(INTC_RTC_ATI, 0);
    sh7305_intc_priority(INTC_RTC_CUI, 0);
    sh7305_intc_priority(INTC_RTC_PRI, 1);

    /* intall the RTC interupt handler */
    sh7305_intc_install_inth_generic(
        0xaa0,
        VHEX_CALL(&sh7305_rtc_periodic_inth)
    );
}

/* __rtc_hsave() : save hardware information */
static void __rtc_hsave(struct rtc_ctx *state)
{
    /* stop as soon as possible RTC counters */
    state->rtc.RCR2.START = SH7305_RTC.RCR2.START;
    SH7305_RTC.RCR2.START = 0;

    state->rtc.RCR1.CF    = SH7305_RTC.RCR1.CF;
    state->rtc.RCR1.CIE   = SH7305_RTC.RCR1.CIE;
    state->rtc.RCR1.AIE   = SH7305_RTC.RCR1.AIE;
    state->rtc.RCR1.AF    = SH7305_RTC.RCR1.AF;

    state->rtc.RCR2.PEF   = SH7305_RTC.RCR2.PEF;
    state->rtc.RCR2.PES   = SH7305_RTC.RCR2.PES;
    state->rtc.RCR2.ADJ   = SH7305_RTC.RCR2.ADJ;

    state->rtc.RCR3.BEN   = SH7305_RTC.RCR3.BEN;
}

/* __rtc_hrestore() : restore hardware information */
static void __rtc_hrestore(struct rtc_ctx *state)
{
    /* stop as soon as possible RTC counters */
    SH7305_RTC.RCR2.START = 0;

    SH7305_RTC.RCR1.CF    = state->rtc.RCR1.CF;
    SH7305_RTC.RCR1.CIE   = state->rtc.RCR1.CIE;
    SH7305_RTC.RCR1.AIE   = state->rtc.RCR1.AIE;
    SH7305_RTC.RCR1.AF    = state->rtc.RCR1.AF;

    SH7305_RTC.RCR2.PEF   = state->rtc.RCR2.PEF;
    SH7305_RTC.RCR2.PES   = state->rtc.RCR2.PES;
    SH7305_RTC.RCR2.ADJ   = state->rtc.RCR2.ADJ;

    SH7305_RTC.RCR3.BEN   = state->rtc.RCR3.BEN;

    SH7305_RTC.RCR2.START = state->rtc.RCR2.START;
}

struct vhex_driver drv_rtc = {
    .name       = "RTC",
    .hpowered   = (void*)&__rtc_hpowered,
    .hpoweron   = (void*)&__rtc_hpoweron,
    .hpoweroff  = (void*)&__rtc_hpoweroff,
    .hsave      = (void*)&__rtc_hsave,
    .hrestore   = (void*)&__rtc_hrestore,
    .configure  = (void*)&__rtc_configure,
    .state_size = sizeof(struct rtc_ctx),
    .flags = {
        .RTC      = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct rtc_drv_interface){
        .rtc_get_time = &sh7305_rtc_get_time,
        .rtc_set_time = &sh7305_rtc_set_time,
    }
};
VHEX_DECLARE_DRIVER(06, drv_rtc);

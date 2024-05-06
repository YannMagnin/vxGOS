#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/driver/mpu/sh/sh7305/cpu.h>
#include <vhex/driver/mpu/sh/sh7305/cpg.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/driver/cpu.h>
#include <vhex/driver.h>
#include <vhex/timer.h>
#include <vhex/timer/interface.h>
#include <vhex/timer/profiling.h>
#include <vhex/timer/fps.h>

#include <string.h>

/* Callbacks for all timers */
vhex_call_t sh7305_tmu_callbacks[9];

/* Arrays of standard and extra timers */
sh7305_tmu_t *TMU = &SH7305_TMU;
sh7305_etmu_t *ETMU = &SH7305_ETMU;

/* Shortcut to set registers that are slow to update */
#define set(lval, rval) do(lval = rval); while(rval != lval)

/* define the private TimerUnit context structure */
struct tmu_ctx {
    struct tmu_state_stored_timer {
        uint32_t TCOR;
        uint32_t TCNT;
        uint16_t TCR;
        uint16_t TSTR;
    } timer[9];
    uint8_t TSTR;
    uint8_t power_bmp;
};

//---
// Internal helpers
//---

uint8_t tmu_status[9] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* valid() : check if the id is valid */
static int invalid(tid_t id)
{
    if (id < 0 || id >= 9)
        return -1;
    return tmu_status[id] == 0;
}

/* conf(): Reserve a fixed timer */
static tid_t reserve(tid_t id, uint32_t delay, int clock)
{
    if(id < 3) {
        /* Refuse to setup timers that are already in use */
        sh7305_tmu_t *T = &TMU[id];
        if(T->TCR.UNIE || SH7305_TMU_TSTR & (1 << id))
            return (-1);

        /* Configure the counter, clear interrupt flag*/
        T->TCOR = delay;
        T->TCNT = delay;
        T->TCR.TPSC = clock;
        set(T->TCR.UNF, 0);

        /* Disable interrupt and count on rising edge (SH7705) */
        T->TCR.UNIE = 0;
        T->TCR.CKEG = 0;
    } else {
        sh7305_etmu_t *T = &ETMU[id-3];
        if(T->TCR.UNIE) return (-1);

        /* No clock input and clock edge here */
        set(T->TCR.UNF, 0);
        set(T->TCOR, delay);
        set(T->TCNT, delay);
        T->TCR.UNIE = 0;
    }
    tmu_status[id] = 1;
    return id;
}

/* conf(): Configure a fixed timer */
static tid_t conf(tid_t id, vhex_call_t call)
{
    if(id < 3) {
        /* Refuse to setup timers that are already in use */
        sh7305_tmu_t *T = &TMU[id];

        /* Enable interrupt and count on rising edge (SH7705) */
        T->TCR.UNIE = 1;
        T->TCR.CKEG = 0;
    } else {
        sh7305_etmu_t *T = &ETMU[id-3];

        /* Enable interrupt */
        T->TCR.UNIE = 1;
    }

    sh7305_tmu_callbacks[id] = call;
    return id;
}

/* control() - start or stop a timer
   @id     Timer ID to configure
   @state  0 to start the timer, 1 to stop it (nothing else!) */
static int control(tid_t id, int state)
{
    if(id < 3) {
        SH7305_TMU_TSTR = (SH7305_TMU_TSTR | (1 << id)) ^ (state << id);
    } else {
        ETMU[id-3].TSTR = state ^ 1;
    }
    return (0);
}

/* available(): Check if a timer is available (UNIE cleared, not running) */
//TODO: proper exposure
int available(int id)
{
    if (id < 0 || id >= 9)
        return (-1);

    if(id < 3) {
        sh7305_tmu_t *T = &TMU[id];
        return !T->TCR.UNIE && !(SH7305_TMU_TSTR & (1 << id));
    }
    sh7305_etmu_t *T = &ETMU[id-3];
    return !T->TCR.UNIE && !T->TSTR;
}

/* compute_delay() - compute a delay constant from a duration in seconds */
static uint32_t compute_delay(int id, uint64_t delay_us, int clock)
{
    struct cpg_clock_frequency cpg_freq;
    uint64_t freq;

    if(id < 3) {
        cpg_clock_freq(&cpg_freq);
        freq = cpg_freq.Pphi_f;
        if(clock == TIMER_Pphi_4)   freq >>= 2;
        if(clock == TIMER_Pphi_16)  freq >>= 4;
        if(clock == TIMER_Pphi_64)  freq >>= 6;
        if(clock == TIMER_Pphi_256) freq >>= 8;
    } else {
        /* ETMU all run on TCLK at 32768 Hz */
        freq = 32768;
    }

    uint64_t product = freq * delay_us;
    return product / 1000000;
}

/* stop_callback(): Empty callback that stops the timer */
static int stop_callback(void)
{
    return TIMER_STOP;
}

//--
// Public timer-API
//---

/* sh7305_tmu_reserve() : reserve a timer */
tid_t sh7305_tmu_reserve(uint64_t delay)
{
    int clock = 0;

    /* Find a matching timer, starting from the slowest timers with the
       smallest interrupt priorities all the way up to TMU0 */
    for(int id = 8; id >= 0; id--)
    {
        if(available(id) == 0)
            continue;

        /* If ID is a TMU, choose a timer prescaler. Assuming the worst
           running Pphi of ~48 MHz, select the finest resolution that
           allows the requested delay to be represented. */
        if(id < 3) {
            uint64_t sec = 1000000;

            /* Pphi/4 until 350 seconds */
            if(delay <= 350 * sec) clock = TIMER_Pphi_4;
            /* Pphi/16 until 1430 seconds */
            else if(delay <= 1430 * sec) clock = TIMER_Pphi_16;
            /* Pphi/64 until 5720 seconds */
            else if(delay <= 5720 * sec) clock = TIMER_Pphi_64;
            /* Pphi/256 otherwise */
            else clock = TIMER_Pphi_256;
        }

        /* Find the delay constant for that timer and clock */
        delay = compute_delay(id, delay, clock);

        return reserve(id, delay, clock);
    }
    return (-1);
}

/* sh7305_tmu_configure() : configure timer */
tid_t sh7305_tmu_configure(uint64_t delay, vhex_call_t call)
{
    tid_t timer = sh7305_tmu_reserve(delay);
    if (timer < 0)
        return -1;

    if(call.function == NULL)
        call = VHEX_CALL(&stop_callback);

    return conf(timer, call);
}

/* sh7305_tmu_start() - start a configured timer */
int sh7305_tmu_start(tid_t id)
{
    return control(id, 0);
}

/* sh7305_tmu_reload() - change a timer's delay constant for next interrupts */
int sh7305_tmu_reload(tid_t id, uint64_t delay)
{
    if (invalid(id))
        return -1;

    if(id < 3) {
        sh7305_tmu_t *T = &TMU[id];
        T->TCOR = compute_delay(id, delay, T->TCR.TPSC);

    } else {
        sh7305_etmu_t *T = &ETMU[id - 3];
        T->TCOR = compute_delay(id, delay, 0);
    }
    return (0);
}

/* sh7305_tmu_pause() - stop a running timer */
int sh7305_tmu_pause(tid_t id)
{
    return control(id, 1);
}

/* sh7305_tmu_stop() - stop and free a timer */
int sh7305_tmu_stop(tid_t id)
{
    if (invalid(id))
        return -1;

    /* Stop the timer and disable UNIE to indicate that it's free */
    sh7305_tmu_pause(id);

    if(id < 3) {
        TMU[id].TCR.UNIE = 0;
        TMU[id].TCR.UNF = 0;
        TMU[id].TCOR = 0xffffffff;
        TMU[id].TCNT = 0xffffffff;
    } else {
        /* Extra timers generate interrupts when TCNT=0 even if TSTR=0.
           We always keep TCOR/TCNT to non-zero values when idle. */
        sh7305_etmu_t *T = &ETMU[id-3];
        T->TCR.UNIE = 0;
        set(T->TCOR, 0xffffffff);
        set(T->TCNT, 0xffffffff);
        set(T->TCR.UNF, 0);
    }
    return 0;
}

/* sh7305_tmu_wait(): Wait for a timer to stop */
int sh7305_tmu_wait(tid_t id)
{
    if (invalid(id) != 0)
        return -1;

    if(id < 3) {
        sh7305_tmu_t *T = &TMU[id];
        /* Sleep only if an interrupt will be there to wake us up */
        while(SH7305_TMU_TSTR & (1 << id)) if(T->TCR.UNIE) cpu_sleep();
    } else {
        sh7305_etmu_t *T = &ETMU[id-3];
        while(T->TSTR) if(T->TCR.UNIE) cpu_sleep();
    }
    return 0;
}

/* sh7305_tmu_spinwait(): Start a timer and actively wait for UNF */
int sh7305_tmu_spinwait(tid_t id)
{
    if (invalid(id))
        return -1;

    if(id < 3) {
        sh7305_tmu_t *T = &TMU[id];
        T->TCR.UNIE = 0;
        sh7305_tmu_start(id);
        while(!T->TCR.UNF) {}
    } else {
        sh7305_etmu_t *T = &ETMU[id-3];
        set(T->TCR.UNIE, 0);
        sh7305_tmu_start(id);
        while(!T->TCR.UNF) {}
    }
    return 0;
}

/* sh7305_tmu_us_spinwait(): Start a timer and actively wait for UNF */
int sh7305_tmu_sleep_us_spinwait(uint64_t us_time)
{
    int timer;

    timer = sh7305_tmu_reserve(us_time);
    if (timer < 0)
        return -1;
    sh7305_tmu_start(timer);
    sh7305_tmu_spinwait(timer);
    sh7305_tmu_stop(timer);
    return 0;
}

/* sh7305_tmu_get_tcnt() : return the timer TCNT register address */
volatile uint32_t *sh7305_tmu_get_tcnt(tid_t id)
{
    if (invalid(id))
        return NULL;

    if (id < 3)
        return &TMU[id].TCNT;
    return &ETMU[id - 3].TCNT;
}

/* sh7305_tmu_set_tcor() : set the timer TCOR value */
void sh7305_tmu_set_tcor(tid_t id, uint32_t value)
{
    if (invalid(id))
        return;

    if (id < 3) {
        TMU[id].TCOR = value;
    } else {
        set(ETMU[id - 3].TCOR, value);
    }
}


//---
// hardware specification
//---


/* __tmu_configure() : configure the SH7305 TMU/ETMU module */
static void __tmu_configure(struct tmu_ctx *s)
{
    memset(s, 0x00, sizeof(struct tmu_ctx));

    /* force enable power */
    s->power_bmp = 3;

    /* prepare timers comtext */
    s->TSTR = 0;
    for(int i = 0; i < 9; i++)
    {
        s->timer[i].TCOR = 0xffffffff;
        s->timer[i].TCNT = 0xffffffff;
        s->timer[i].TCR  = 0;
        s->timer[i].TSTR = 0;
    }

    /* install the TMUs interupt handler */
    extern uint32_t sh7305_inth_tmu;
    sh7305_intc_install_inth_gate(0x400, &sh7305_inth_tmu, 96);

    /* install all ETMUx interrupt handler

      The interruptions handling for the ETMU is more complexe that the
      "classical" timer (TMU) because, instead of the TMU that each
      interrupt vector is grouped (TMU0:0x400, TMU1:0x420, TMU2:0x440), each
      ETMUx interrupt vector is not followed so we should relocalize and
      update each interrut gate.

      We know that the ETMU4 (which have the interrupt vector 0xd00) is
      followed by excatly 3 free gates (where not interrupt exist). We will
      use this place to store all the ETMUx interrupt logic. */
    extern uint32_t sh7305_inth_etmu4;
    extern uint32_t sh7305_inth_etmux;
    void *h4;
    void *h;

    uint16_t etmu_evt[6] = { 0x9e0, 0xc20, 0xc40, 0x900, 0xd00, 0xfa0 };
    h4 = sh7305_intc_install_inth_gate(etmu_evt[4], &sh7305_inth_etmu4, 96);

    for (int i = 0; i < 6; ++i) {
        /* skip the core ETMUx core handler */
        if (i == 4)
            continue;

        /* install the default interrupt handler */
        h = sh7305_intc_install_inth_gate(
            etmu_evt[i],
            &sh7305_inth_etmux,
            32
        );

        /* Timer ID, used for sh7305_tmu_stop() after the callback */
        *(uint16_t *)(h + 16) = i + 3;
        /* ETMU4 interrupt handler address, used to jump */
        *(uint32_t *)(h + 20) = (uintptr_t)h4;
        /* Pointer to the callback */
        *(uint32_t *)(h + 24) = (uintptr_t)&sh7305_tmu_callbacks[i + 3];
        /* TCR address to acknowledge the interrupt */
        *(uint32_t *)(h + 28) = (uintptr_t)&ETMU[i].TCR;
    }

    /* Enable TMU0 at level 13, TMU1 at level 11, TMU2 at level 9 */
    sh7305_intc_priority(INTC_TMU_TUNI0, 13);
    sh7305_intc_priority(INTC_TMU_TUNI1, 11);
    sh7305_intc_priority(INTC_TMU_TUNI2, 9);

    /* Enable the extra TMUs at level 7 */
    sh7305_intc_priority(INTC_ETMU_TUNI0, 7);
    sh7305_intc_priority(INTC_ETMU_TUNI1, 7);
    sh7305_intc_priority(INTC_ETMU_TUNI2, 7);
    sh7305_intc_priority(INTC_ETMU_TUNI3, 7);
    sh7305_intc_priority(INTC_ETMU_TUNI4, 7);
    sh7305_intc_priority(INTC_ETMU_TUNI5, 7);
}

/* __tmu_hsave() : save hardware information */
static void __tmu_hsave(struct tmu_ctx *s)
{
    struct tmu_state_stored_timer *c;

    /* power workaround */
    s->power_bmp = 0;

    /* classic timer */
    if (SH7305_POWER.MSTPCR0.TMU == 0)
    {
        s->TSTR = SH7305_TMU_TSTR;
        for(int i = 0; i < 3; i++)
        {
            s->timer[i].TCOR = TMU[i].TCOR;
            s->timer[i].TCNT = TMU[i].TCNT;
            s->timer[i].TCR  = TMU[i].TCR.word;
        }
        s->power_bmp |= 1;
    }

    /* extra timer */
    if (SH7305_POWER.MSTPCR2.ETMU == 0)
    {
        c = &s->timer[3];
        for(int i = 0; i < 6; i++)
        {
            /* Don't snapshot an interrupt state, because the timer
               state is sometimes garbage protected by a masked
               interrupt. */
            c[i].TCOR = ETMU[i].TCOR ? ETMU[i].TCOR : 0xffffffff;
            c[i].TCNT = ETMU[i].TCNT ? ETMU[i].TCNT : c->TCOR;
            c[i].TCR  = ETMU[i].TCR.byte & 0xd;
            c[i].TSTR = ETMU[i].TSTR;
        }
        s->power_bmp |= 2;
    }
}

/* __tmu_hrestore() : restore hadware information */
static void __tmu_hrestore(struct tmu_ctx *s)
{
    struct tmu_state_stored_timer const *c;

    /* power workaround */
    SH7305_POWER.MSTPCR0.TMU  = (s->power_bmp & 1) ? 0 : 1;
    SH7305_POWER.MSTPCR2.ETMU = (s->power_bmp & 2) ? 0 : 1;

    /* classic timer */
    if (SH7305_POWER.MSTPCR0.TMU == 0)
    {
        SH7305_TMU_TSTR = 0;
        for(int i = 0; i < 3; i++)
        {
            TMU[i].TCOR = s->timer[i].TCOR;
            TMU[i].TCNT = s->timer[i].TCNT;
            TMU[i].TCR.word = s->timer[i].TCR;
        }
        SH7305_TMU_TSTR = s->TSTR;
    }

    /* extra timer */
    if (SH7305_POWER.MSTPCR2.ETMU == 0)
    {
        c = &s->timer[3];
        for(int i = 0; i < 6; i++)
        {
            set(ETMU[i].TCOR, c[i].TCOR);
            set(ETMU[i].TCNT, c[i].TCNT);
            set(ETMU[i].TCR.byte, c[i].TCR);
            set(ETMU[i].TSTR, c[i].TSTR);
        }
    }
}

//@note : we use the TMU and the ETMU hardware module, so we have two
// alimentation to check. If one of them is powered, return that the whole
// driver is powered. The __tmu_hsave() and __tmu_hresotre() will handle the
// power management
static bool __tmu_hpowered(void)
{
    return (SH7305_POWER.MSTPCR0.TMU == 0 || SH7305_POWER.MSTPCR2.ETMU == 0);
}

static int __tmu_hpoweron(void)
{
    SH7305_POWER.MSTPCR0.TMU = 0;
    SH7305_POWER.MSTPCR2.ETMU = 0;
    return 0;
}

static int __tmu_hpoweroff(void)
{
    SH7305_POWER.MSTPCR0.TMU = 1;
    SH7305_POWER.MSTPCR2.ETMU = 1;
    return 0;
}

/* declare the TMU driver */

struct vhex_driver drv_tmu = {
    .name       = "TMU",
    .hpowered   = (void*)&__tmu_hpowered,
    .hpoweron   = (void*)&__tmu_hpoweron,
    .hpoweroff  = (void*)&__tmu_hpoweroff,
    .hsave      = (void*)&__tmu_hsave,
    .hrestore   = (void*)&__tmu_hrestore,
    .configure  = (void*)&__tmu_configure,
    .state_size = sizeof(struct tmu_ctx),
    .flags = {
        .TIMER  = 1,
        .SHARED = 0,
        .UNUSED = 0
    },
    .module_data = &(struct timer_drv_interface){
        /* timer API */
        .timer_reserve        = &sh7305_tmu_reserve,
        .timer_configure      = &sh7305_tmu_configure,
        .timer_start          = &sh7305_tmu_start,
        .timer_pause          = &sh7305_tmu_pause,
        .timer_stop           = &sh7305_tmu_stop,
        .timer_wait           = &sh7305_tmu_wait,
        .timer_spinwait       = &sh7305_tmu_spinwait,
        .timer_reload         = &sh7305_tmu_reload,
        /* profiling API */
        .timer_prof_init      = &sh7305_tmu_prof_init,
        .timer_prof_enter     = &sh7305_tmu_prof_enter,
        .timer_prof_enter_rec = &sh7305_tmu_prof_enter_rec,
        .timer_prof_leave_rec = &sh7305_tmu_prof_leave_rec,
        .timer_prof_leave     = &sh7305_tmu_prof_leave,
        .timer_prof_time      = &sh7305_tmu_prof_time,
        .timer_prof_quit      = &sh7305_tmu_prof_quit,
        /* fps API */
        .timer_fps_init       = &sh7305_tmu_fps_init,
        .timer_fps_sync       = &sh7305_tmu_fps_sync,
        .timer_fps_quit       = &sh7305_tmu_fps_quit,
    }
};
VHEX_DECLARE_DRIVER(03, drv_tmu);

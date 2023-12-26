#include <vhex/timer.h>
#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/driver/mpu/sh/sh7305/cpg.h>
#include <vhex/driver/cpu.h>

uint32_t volatile *tmu_prof_tcnt = NULL;
static int tmu_prof_counter = 0;
static int tmu_prof_timer = -1;

extern int available(int id);

/* defined at <tmu.c> */
extern sh7305_tmu_t *TMU;

/* Shortcut to set registers that are slow to update */
#define set(lval, rval) do(lval = rval); while(rval != lval)

//---
// Public profiling API
//---

/* sh7305_tmu_prof_make() : initialise a new  */
int sh7305_tmu_prof_init(timer_prof_t *prof)
{
    cpu_atomic_start();

    if (tmu_prof_tcnt == NULL) {
        tmu_prof_timer = -1;
        for(int t = 0; t < 3; t++)
        {
            if (!available(t))
                continue;
            tmu_prof_timer = t;
            tmu_prof_tcnt = &TMU[t].TCNT;

            TMU[t].TCOR = 0xffffffff;
            TMU[t].TCNT = 0xffffffff;
            TMU[t].TCR.TPSC = TIMER_Pphi_4;
            set(TMU[t].TCR.UNF, 0);
            TMU[t].TCR.UNIE = 0;
            TMU[t].TCR.CKEG = 0;

            SH7305_TMU_TSTR |= 1 << t;
            break;
        }
        if (tmu_prof_timer < 0) {
            cpu_atomic_end();
            return -1;
        }
    }

    if (prof != NULL) {
        prof->rec = 0;
        prof->elapsed = 0;
        prof->anchor = *tmu_prof_tcnt;
    }

    tmu_prof_counter += 1;

    cpu_atomic_end();
    return 0;
}

/* sh7305_tmu_prof_enter(): Start counting time for a function */
void sh7305_tmu_prof_enter(timer_prof_t *prof)
{
    if (prof == NULL)
        return;

    prof->anchor = 0;
    if (tmu_prof_tcnt != NULL)
        prof->anchor = *tmu_prof_tcnt;
}

/* sh7305_tmu_prof_enter_rec(): Start counting time for a recursive function */
void sh7305_tmu_prof_enter_rec(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    if (prof->rec == 0)
        sh7305_tmu_prof_enter(prof);
    prof->rec += 1;
}

/* sh7305_tmu_prof_leave_rec(): Start counting time for a recursive function */
void sh7305_tmu_prof_leave_rec(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    if (prof->rec <= 0) {
        prof->rec = 0;
        return;
    }
    prof->rec -= 1;
    if (prof->rec == 0)
        sh7305_tmu_prof_leave(prof);
}

/* sh7305_tmu_prof_leave(): Stop counting time for a function */
void sh7305_tmu_prof_leave(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    if (tmu_prof_tcnt == NULL)
        return;

    uint32_t snapshot = *tmu_prof_tcnt;

    prof->elapsed += prof->anchor - snapshot;
    prof->anchor = snapshot;
}


/* sh7305_prof_time(): Time spent in a given context, in microseconds */
uint32_t sh7305_tmu_prof_time(timer_prof_t *prof)
{
    struct cpg_clock_frequency cpg_freq;

    cpg_clock_freq(&cpg_freq);
    return ((uint64_t)prof->elapsed * 4 * 1000000) / cpg_freq.Pphi_f;
}

/* sh7305_tmu_prof_quit() : uninit the profoling object */
int sh7305_tmu_prof_quit(timer_prof_t *prof)
{
    (void)prof;

    cpu_atomic_start();
    if (--tmu_prof_counter <= 0) {
        if(tmu_prof_timer == 0)
            sh7305_tmu_stop(tmu_prof_timer);
        tmu_prof_timer = -1;
        tmu_prof_tcnt = NULL;
        tmu_prof_counter = 0;
    }
    cpu_atomic_end();
    return 0;
}

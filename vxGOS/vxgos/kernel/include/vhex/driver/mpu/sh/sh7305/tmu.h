#ifndef __VHEX_MPU_SH7305_TMU__
# define __VHEX_MPU_SH7305_TMU__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>
#include <vhex/defs/call.h>

//---
//  SH7305 Timer Unit. Refer to:
//    "Renesas SH7724 User's Manual: Hardware"
//    Section 20: "Timer Unit (TMU)"
//---

struct sh7305_tmu
{
    uint32_t TCOR;              /* Constant register */
    uint32_t TCNT;              /* Counter register, counts down */
    word_union(TCR,
        uint16_t            :7;
        uint16_t UNF        :1; /* Underflow flag */
        uint16_t            :2;
        uint16_t UNIE       :1; /* Underflow interrupt enable */
        uint16_t CKEG       :2; /* Input clock edge */
        uint16_t TPSC       :3; /* Timer prescaler (input clock) */
    );
    pad(2);
} VPACKED(4);
typedef volatile struct sh7305_tmu sh7305_tmu_t;

#define SH7305_TMU_TSTR (*(volatile uint8_t *)0xa4490004)
#define SH7305_TMU      (*(sh7305_tmu_t     *)0xa4490008)

//---
//  SH7305 Extra Timer Unit. No official documentation exists.
//---

/* sh7305_etmu_t - extra timers on SH7337, SH7355 and SH7305 */
struct sh7305_etmu
{
    uint8_t TSTR;               /* Only bit 0 is used */
    pad(3);

    uint32_t TCOR;              /* Constant register */
    uint32_t TCNT;              /* Counter register */

    byte_union(TCR,
        uint8_t             :6;
        uint8_t UNF         :1; /* Underflow flag */
        uint8_t UNIE        :1; /* Underflow interrupt enable */
    );
    pad(19);

} VPACKED(4);
typedef volatile struct sh7305_etmu sh7305_etmu_t;

#define SH7305_ETMU (*(sh7305_etmu_t *)0xa44d0030)


//---
// internal API
//---

/* Clock input

   Standard TMU can count at different speeds. A fast speed offers more
   precision but a slower speed offers longer delays. vhex automatically
   selects suitable speeds by default.

   If you want something very particular, you can add (with + or |) a prescaler
   value to a chosen ID in timer_configure() to request that specific value.
   The default prescaler if the ID is fixed is TIMER_Pphi_4. */
enum {
    TIMER_Pphi_4   = 0x00,
    TIMER_Pphi_16  = 0x10,
    TIMER_Pphi_64  = 0x20,
    TIMER_Pphi_256 = 0x30,
};

#include <vhex/timer.h>

/* timer */

/* sh7305_tmu_reserve() : reserve a timer */
extern tid_t sh7305_tmu_reserve(uint64_t delay);

/* sh7305_tmu_configure() : configure timer */
extern tid_t sh7305_tmu_configure(uint64_t delay, vhex_call_t callback);

/* sh7305_tmu_start() - start a configured timer */
extern int sh7305_tmu_start(tid_t id);

/* sh7305_tmu_reload() - change a timer's delay constant for next interrupts */
extern int sh7305_tmu_reload(tid_t id, uint64_t delay);

/* sh7305_tmu_pause() - stop a running timer */
extern int sh7305_tmu_pause(tid_t id);

/* sh7305_tmu_stop() - stop and free a timer */
extern int sh7305_tmu_stop(tid_t id);

/* sh7305_tmu_wait(): Wait for a timer to stop */
extern int sh7305_tmu_wait(tid_t id);

/* sh7305_tmu_spinwait(): Start a timer and actively wait for UNF */
extern int sh7305_tmu_spinwait(tid_t id);

/* sh7305_tmu_get_tcnt() : return the timer TCNT register address */
extern volatile uint32_t *sh7305_tmu_get_tcnt(tid_t id);

/* sh7305_tmu_set_tcor() : set the timer TCOR value */
extern void sh7305_tmu_set_tcor(tid_t id, uint32_t value);

/* sh7305_tmu_us_spinwait(): Start a timer and actively wait for UNF */
extern int sh7305_tmu_sleep_us_spinwait(uint64_t us_time);

/* profiling */

/* sh7305_tmu_prof_make() : initialise a new  */
extern int sh7305_tmu_prof_init(timer_prof_t *prof);

/* sh7305_tmu_prof_enter(): Start counting time for a function */
extern void sh7305_tmu_prof_enter(timer_prof_t *prof);

/* sh7305_tmu_prof_enter_rec(): Start counting time for a recursive function */
extern void sh7305_tmu_prof_enter_rec(timer_prof_t *prof);

/* sh7305_tmu_prof_leave_rec(): Start counting time for a recursive function */
extern void sh7305_tmu_prof_leave_rec(timer_prof_t *prof);

/* sh7305_tmu_prof_leave(): Stop counting time for a function */
extern void sh7305_tmu_prof_leave(timer_prof_t *prof);

/* sh7305_prof_time(): Time spent in a given context, in microseconds */
extern uint32_t sh7305_tmu_prof_time(timer_prof_t *prof);

/* sh7305_tmu_prof_quit() : uninit the profoling object */
extern int sh7305_tmu_prof_quit(timer_prof_t *prof);

/* FSP */

#include <vhex/timer/fps.h>

/* sh7305_tmu_fps_init() : initialize fps object */
extern void sh7305_tmu_fps_init(fps_t *fps, int fps_target);

/* sh7305_tmu_fps_sync() : compute frame statistic and wait whanted FPS */
extern void sh7305_tmu_fps_sync(fps_t *fps);

/* sh7305_fps_quit() : uninit the fps object */
extern void sh7305_tmu_fps_quit(fps_t *fps);

#endif /* __VHEX_MPU_SH7305_TMU__ */

#ifndef __VHEX_TIMER__
#define __VHEX_TIMER__

#include <vhex/timer/types.h>
#include <vhex/defs/call.h>

/* TIMER_DELAY_MS() : convert ms into us */
#define TIMER_DELAY_MS(ms)      (ms * 1000)
#define TIMER_DELAY_SEC(sec)    (sec * 1000000)

/* returned status from a callback */
enum {
    TIMER_CONTINUE  = 0,
    TIMER_STOP      = 1,
};

/* timer_configure(): Reserve and configure a timer

   This function finds and configures a timer (without starting it). On
   success, it returns the ID of the configured timer, which is used in all
   other timer functions. If no timer matching the requested settings is
   available, this function returns -1.

   When started, the configured timer will run for the requested delay and call
   the supplied callback function at the end of this delay. The callback
   function can then decide whether to leave the timer running (and be called
   again after the same delay) or stop the timer. */
extern tid_t timer_configure(uint64_t delay_us, vhex_call_t callback);

/* timer_reserve() : reserve a timer

   The timer can be used normaly, but no callback nor interruption will occur.
   This primitive is usefull if you whant to have only the timer counter */
extern tid_t timer_reserve(uint64_t delay_us);

/* timer_start(): Start a configured timer */
extern int timer_start(tid_t timer);

/* timer_pause(): Pause a timer without freeing it */
extern int timer_pause(tid_t timer);

/* timer_stop(): Stop and free a timer */
extern int timer_stop(tid_t timer);

/* timer_wait(): Wait for a timer to stop
   Waits until the timer pauses or stops. If the timer is not running, returns
   immediately. Even after timer_wait(), the timer may not be available since
   it may have only paused. If the timer never stops, you're in trouble. */
extern int timer_wait(tid_t timer);

/* timer_spinwait(): Start a timer and actively wait
   Waits until the timer has finished its countdown, without sleeping. This is
   useful for delays in driver code that is run when interrupts are disabled.
   Interrupt are disabled before starting the timer and waiting, so the callback
   is never called. */
extern int timer_spinwait(tid_t timer);

/* timer_reload(): Change a timer's delay constant for next interrupts */
extern int timer_reload(tid_t timer, uint64_t delay);

#endif /* __VHEX_TIMER__ */

#ifndef __VHEX_PROFILING__
# define __VHEX_PROFILING__

#include <vhex/timer/types.h>


//---
//  Runtime time measurement API
//---

/* timer_prof_init(): Create a new context object */
extern int timer_prof_init(timer_prof_t *prof);

/* timer_prof_enter(): Start counting time for a function
   This fucntion should be called at the start of the context scope */
extern void timer_prof_enter(timer_prof_t *prof);

/* timer_prof_leave(): Stop counting time for a function
   This should be called at the end of the context scope. */
extern void timer_prof_leave(timer_prof_t *prof);

/* Variant of timer_prof_enter()/timer_prof_leave() for recursive contexts */
extern void timer_prof_enter_rec(timer_prof_t *prof);
extern void timer_prof_leave_rec(timer_prof_t *prof);

/* timer_prof_exec(): Measure a single block of (non-recurive) code
   This operation can be used when profiling is not required, and instead
   its is used to measure the performance of a single bit of code. Use it
   like this:

       uint32_t elasped_us = prof_exec({
           exec_code();
       }); */
#define timer_prof_exec(code) ({                \
    timer_prof_t prof;                          \
    timer_prof_init(&prof);                     \
    timer_prof_enter(&prof);                    \
    code;                                       \
    timer_prof_leave(&prof);                    \
    uint32_t time = timer_prof_time(&prof);     \
    timer_prof_quit(&prof);                     \
    time;                                       \
})

/* timer_prof_quit() : uninit timer_prof object */
extern int timer_prof_quit(timer_prof_t *prof);


//---
//  Post-measurement analysis
//---

/* timer_prof_time(): Time spent in a given context, in microseconds
   Should only be called when the context is not currently executing. */
extern uint32_t timer_prof_time(timer_prof_t *prof);


#endif /* __VHEX_PROFILING__ */

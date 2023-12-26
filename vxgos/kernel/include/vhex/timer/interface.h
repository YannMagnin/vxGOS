#ifndef __VHEX_TIMER_INTERFACE__
# define __VHEX_TIMER_INTERFACE__

#include <vhex/defs/call.h>
#include <vhex/timer/types.h>
#include <vhex/timer/fps.h>

/* timer_drv_interface - driver interface */
struct timer_drv_interface
{
    /* timer API */
    tid_t (*timer_reserve)(uint64_t delay_us);
    tid_t (*timer_configure)(uint64_t delay_us, vhex_call_t callback);
    int (*timer_start)(tid_t timer);
    int (*timer_pause)(tid_t timer);
    int (*timer_stop)(tid_t timer);
    int (*timer_wait)(tid_t timer);
    int (*timer_spinwait)(tid_t timer);
    int (*timer_reload)(tid_t timer, uint64_t delay);
    /* profiling */
    int (*timer_prof_init)(timer_prof_t *prof);
    void (*timer_prof_enter)(timer_prof_t *prof);
    void (*timer_prof_enter_rec)(timer_prof_t *prof);
    void (*timer_prof_leave_rec)(timer_prof_t *prof);
    void (*timer_prof_leave)(timer_prof_t *prof);
    uint32_t (*timer_prof_time)(timer_prof_t *prof);
    int (*timer_prof_quit)(timer_prof_t *prof);
    /* FPS */
    void (*timer_fps_init)(fps_t *fps, int fps_target);
    void (*timer_fps_sync)(fps_t *fps);
    void (*timer_fps_quit)(fps_t *fps);
};

#endif /* __VHEX_TIMER_INTERFACE__ */

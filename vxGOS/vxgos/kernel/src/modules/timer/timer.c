#include <vhex/timer/interface.h>
#include <vhex/driver.h>
#include <vhex/module.h>
#include <vhex/timer.h>

#include <string.h>

/* internal timer information */
struct {
    struct timer_drv_interface driver;
} timer_info;

//---
// user-level timer API
//---

/* timer_reserve(): Reserve and configure a timer */
tid_t timer_reserve(uint64_t delay_us)
{
    if (timer_info.driver.timer_reserve != NULL)
        return (timer_info.driver.timer_reserve(delay_us));
    return (-1);
}

/* timer_configure(): Reserve and configure a timer */
tid_t timer_configure(uint64_t delay_us, vhex_call_t callback)
{
    if (timer_info.driver.timer_configure != NULL)
        return (timer_info.driver.timer_configure(delay_us, callback));
    return (-1);
}

/* timer_start(): Start a configured timer */
int timer_start(tid_t timer)
{
    if (timer_info.driver.timer_start != NULL)
        return (timer_info.driver.timer_start(timer));
    return (-1);
}

/* timer_pause(): Pause a timer without freeing it */
int timer_pause(tid_t timer)
{
    if (timer_info.driver.timer_pause != NULL)
        return (timer_info.driver.timer_pause(timer));
    return (-1);
}

/* timer_stop(): Stop and free a timer */
int timer_stop(tid_t timer)
{
    if (timer_info.driver.timer_stop != NULL)
        return (timer_info.driver.timer_stop(timer));
    return (-1);
}

/* timer_wait(): Wait for a timer to stop */
int timer_wait(tid_t timer)
{
    if (timer_info.driver.timer_wait != NULL)
        return (timer_info.driver.timer_wait(timer));
    return (-1);
}

/* timer_spinwait(): Start a timer and actively wait */
int timer_spinwait(tid_t timer)
{
    if (timer_info.driver.timer_spinwait != NULL)
        return (timer_info.driver.timer_spinwait(timer));
    return (-1);
}

/* timer_reload(): Change a timer's delay constant for next interrupts */
int timer_reload(tid_t timer, uint64_t delay_us)
{
    if (timer_info.driver.timer_reload != NULL)
        return (timer_info.driver.timer_reload(timer, delay_us));
    return (-1);
}



//---
// Kernel module information
//---


/* __timer_init() : initialize the timer module */
static void __timer_init(void)
{
    memset(&timer_info, 0x00, sizeof(timer_info));

    struct vhex_driver *driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.TIMER) {
            memcpy(
                &timer_info.driver,
                driver[i].module_data,
                sizeof(struct timer_drv_interface)
            );
            break;
        }
    }
}

/* __timer_quit() : uninitialize the timer module */
static void __timer_quit(void)
{
    ;
}



/* declare the timer module */

struct vhex_module mod_timer = {
    .name = "timer",
    .init = &__timer_init,
    .quit = &__timer_quit,
};
VHEX_DECLARE_MODULE(04, mod_timer);

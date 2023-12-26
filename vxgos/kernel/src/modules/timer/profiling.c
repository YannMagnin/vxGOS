#include <vhex/timer/profiling.h>
#include <vhex/timer/interface.h>

#include <string.h>

/* internal timer information */
extern struct {
    struct timer_drv_interface driver;
} timer_info;

//---
// user-level profiling API
//---

/* timer_prof_init(): Create a new context object */
int timer_prof_init(timer_prof_t *prof)
{
    memset(prof, 0x00, sizeof(timer_prof_t));
    if (timer_info.driver.timer_prof_init != NULL)
        return (timer_info.driver.timer_prof_init(prof));
    return (-1);
}

/* timer_prof_enter(): Start counting time for a function */
void timer_prof_enter(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_enter != NULL)
        timer_info.driver.timer_prof_enter(prof);
}

/* timer_prof_leave(): Stop counting time for a function */
void timer_prof_leave(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_leave != NULL)
        timer_info.driver.timer_prof_leave(prof);
}

/* timer_prof_enter_rec(): Start counting time for a recursive function */
void timer_prof_enter_rec(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_enter_rec != NULL)
        timer_info.driver.timer_prof_enter_rec(prof);
}

/* timer_prof_leave_rec(): Stop counting time for a recursive function */
void timer_prof_leave_rec(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_leave_rec != NULL)
        timer_info.driver.timer_prof_leave_rec(prof);
}

/* timer_prof_quit() : uninit timer_prof object */
int timer_prof_quit(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_quit != NULL)
        return (timer_info.driver.timer_prof_quit(prof));
    return (-1);
}

/* timer_prof_time(): Time spent in a given context, in microseconds */
uint32_t timer_prof_time(timer_prof_t *prof)
{
    if (timer_info.driver.timer_prof_time != NULL)
        return (timer_info.driver.timer_prof_time(prof));
    return (-1);
}

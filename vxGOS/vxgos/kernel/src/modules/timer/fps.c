#include <vhex/timer/profiling.h>
#include <vhex/timer/interface.h>

/* internal timer information */
extern struct {
    struct timer_drv_interface driver;
} timer_info;

//---
// user-level fps API
//---


/* timer_fps_init() : initialize fps object */
void timer_fps_init(fps_t *fps, int target_ticks)
{
    if (timer_info.driver.timer_fps_init != NULL)
        timer_info.driver.timer_fps_init(fps, target_ticks);
}

/* timer_fps_sync() : try to syncronize to X frame per seconds */
void timer_fps_sync(fps_t *fps)
{
    if (timer_info.driver.timer_fps_sync != NULL)
        timer_info.driver.timer_fps_sync(fps);
}

/* timer_fps_quit() : uninit fps object */
void timer_fps_quit(fps_t *fps)
{
    if (timer_info.driver.timer_fps_quit != NULL)
        timer_info.driver.timer_fps_quit(fps);
}

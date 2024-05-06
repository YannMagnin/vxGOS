#ifndef __VHEX_TIMER_FPS__
# define __VHEX_TIMER_FPS__

#include <vhex/timer/types.h>

/* timer_fps - fps object */
struct timer_fps {
    int fps;
    int fps_real;
    int fps_target;
    uint32_t render;
    int32_t margin;
    uint32_t objectif;
    uint32_t anchor;
};
typedef struct timer_fps fps_t;

/* timer_fps_init() : initialize fps object */
extern void timer_fps_init(fps_t *fps, int fps_target);

/* timer_fps_sync() : try to syncronize to X frame per seconds */
extern void timer_fps_sync(fps_t *fps);

/* timer_fps_quit() : uninit fps object */
extern void timer_fps_quit(fps_t *fps);

#endif

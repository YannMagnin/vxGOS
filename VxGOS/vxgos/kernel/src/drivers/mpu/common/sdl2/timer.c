#include <vhex/driver.h>
#include <vhex/timer/interface.h>

#include <SDL2/SDL.h>

//---
// Internal
//---

/* internal timer cache */
static struct {
    bool used;
    vhex_call_t call;
    uint32_t delay;
    SDL_TimerID id;
} timer_cache[9];

static uint32_t __sdl_tmu_nexus(uint32_t interval, int id)
{
    (void)interval;
    if (timer_cache[id].call.function != NULL) {
        vhex_call(timer_cache[id].call);
    }
    return timer_cache[id].delay;
}

//---
// Public timer API
// FIXME : atomic operation
//---

/* sdl_tmu_reserve() : reserve a timer */
tid_t sdl_tmu_reserve(uint64_t delay)
{
    for (int i = 0; i < 9; ++i) {
        if (timer_cache[i].used == true)
            continue;
        timer_cache[i].used = true;
        timer_cache[i].call = VHEX_CALL_NULL;
        timer_cache[i].delay = delay / 1000;
        timer_cache[i].id = 0;
        return i;
    }
    return -1;
}

/* sdl_tmu_configure() : configure timer */
tid_t sdl_tmu_configure(uint64_t delay, vhex_call_t call)
{
    tid_t id;

    id = sdl_tmu_reserve(delay);
    if (id < 0)
        return id;

    memcpy(&timer_cache[id].call, &call, sizeof(vhex_call_t));
    return id;
}


/* sdl_tmu_start() - start a configured timer */
int sdl_tmu_start(tid_t id)
{
    if (id < 0 || id >= 9)       return -1;
    if (timer_cache[id].id != 0) return -2;

    timer_cache[id].id = SDL_AddTimer(
        timer_cache[id].delay,
        (void*)&__sdl_tmu_nexus,
        (void*)(uintptr_t)id
    );
    return 0;
}

/* sdl_tmu_reload() - change a timer's delay constant for next interrupts */
int sdl_tmu_reload(tid_t id, uint64_t delay)
{
    if (id < 0 || id >= 9)
        return -1;

    timer_cache[id].delay = delay / 1000;
    return 0;
}

/* sdl_tmu_pause() - stop a running timer */
int sdl_tmu_pause(tid_t id)
{
    if (id < 0 || id >= 9)
        return -1;

    SDL_RemoveTimer(timer_cache[id].id);
    timer_cache[id].id = 0;
    return 0;
}

/* sdl_tmu_stop() - stop and free a timer */
int sdl_tmu_stop(tid_t id)
{
    if (id < 0 || id >= 9)
        return -1;

    SDL_RemoveTimer(timer_cache[id].id);
    timer_cache[id].id = 0;
    timer_cache[id].used = false;
    return 0;
}

//---
// Profiling API
//---

/* sdl_tmu_prof_make() : initialise a new profiling timer  */
int sdl_tmu_prof_init(timer_prof_t *prof)
{
    prof->anchor = SDL_GetTicks();
    prof->elapsed = 0;
    prof->rec = 0;
    return 0;
}

/* sdl_tmu_prof_enter(): Start counting time for a function */
void sdl_tmu_prof_enter(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    prof->anchor = SDL_GetTicks();
}

/* sdl_tmu_prof_enter_rec(): Start counting time for a recursive function */
void sdl_tmu_prof_enter_rec(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    if (prof->rec == 0)
        sdl_tmu_prof_enter(prof);
    prof->rec += 1;
}

/* sdl_tmu_prof_leave(): Stop counting time for a function */
void sdl_tmu_prof_leave(timer_prof_t *prof)
{
    if (prof == NULL)
        return;

    uint32_t snapshot = SDL_GetTicks();

    prof->elapsed += snapshot - prof->anchor;
    prof->anchor = snapshot;
}

/* sdl_tmu_prof_leave_rec(): Start counting time for a recursive function */
void sdl_tmu_prof_leave_rec(timer_prof_t *prof)
{
    if (prof == NULL)
        return;
    if (prof->rec <= 0) {
        prof->rec = 0;
        return;
    }
    prof->rec -= 1;
    if (prof->rec == 0)
        sdl_tmu_prof_leave(prof);
}

/* sdl_prof_time(): Time spent in a given context, in microseconds */
uint32_t sdl_tmu_prof_time(timer_prof_t *prof)
{
    if (prof == NULL)
        return 0;
    return prof->elapsed * 1000;
}

/* sdl_tmu_prof_quit() : uninit the profoling object */
int sdl_tmu_prof_quit(timer_prof_t *prof)
{
    (void)prof;
    return 0;
}

//---
// FPS primitives
//---

/* sh7305_tmu_fps_init() : initialize fps object */
void sdl_tmu_fps_init(fps_t *fps, int fps_target)
{
    memset(fps, 0x00, sizeof(fps_t));
    fps->anchor = SDL_GetTicks();
    fps->objectif = 1000 / fps_target;
    fps->fps_target = fps_target;
}

/* sdl_tmu_fps_sync() : compute frame statistic and wait whanted FPS */
void sdl_tmu_fps_sync(fps_t *fps)
{
    uint32_t snapshot;
    uint32_t target_ticks;

    snapshot = SDL_GetTicks();

    fps->render = snapshot - fps->anchor;
    fps->margin = fps->objectif - fps->render;

    fps->fps_real = 1000;
    if (fps->render > 0)
        fps->fps_real /= fps->render;

    fps->fps = fps->fps_real;
    if (fps->margin > 0)
        fps->fps = fps->fps_target;

    target_ticks = snapshot + fps->objectif;
    while (1) {
        if (SDL_GetTicks() >= target_ticks)
            break;
    }

    fps->anchor = SDL_GetTicks();
}

/* sdl_fps_quit() : uninit the fps object */
void sdl_tmu_fps_quit(fps_t *fps)
{
    (void)fps;
}

//---
// fake driver specification
//---

static void __tmu_configure(void)
{
    printf("[drv] TMU: fake driver initialized\n");
    for (int i = 0; i < 9; ++i) {
        timer_cache[i].used = false;
        timer_cache[i].id = 0;
    }
}

static void __tmu_hsave(void)
{
    // Nothing to do, this is a fake driver
    ;
}

static void __tmu_hrestore(void)
{
    // Nothing to do, this is a fake driver
    ;
}

struct vhex_driver drv_tmu = {
    .name       = "SDL2 Timer",
    .hsave      = (void*)&__tmu_hsave,
    .hrestore   = (void*)&__tmu_hrestore,
    .configure  = (void*)&__tmu_configure,
    .state_size = 4,
    .flags = {
        .TIMER  = 1,
        .SHARED = 0,
        .UNUSED = 0
    },
    .module_data = &(struct timer_drv_interface){
        /* timer API */
        .timer_reserve        = &sdl_tmu_reserve,
        .timer_configure      = &sdl_tmu_configure,
        .timer_start          = &sdl_tmu_start,
        .timer_pause          = &sdl_tmu_pause,
        .timer_stop           = &sdl_tmu_stop,
        .timer_wait           = NULL,
        .timer_spinwait       = NULL,
        .timer_reload         = &sdl_tmu_reload,
        /* profiling API */
        .timer_prof_init      = &sdl_tmu_prof_init,
        .timer_prof_enter     = &sdl_tmu_prof_enter,
        .timer_prof_enter_rec = &sdl_tmu_prof_enter_rec,
        .timer_prof_leave_rec = &sdl_tmu_prof_leave_rec,
        .timer_prof_leave     = &sdl_tmu_prof_leave,
        .timer_prof_time      = &sdl_tmu_prof_time,
        .timer_prof_quit      = &sdl_tmu_prof_quit,
        /* fps API */
        .timer_fps_init       = &sdl_tmu_fps_init,
        .timer_fps_sync       = &sdl_tmu_fps_sync,
        .timer_fps_quit       = &sdl_tmu_fps_quit,
    }
};
VHEX_DECLARE_DRIVER(03, drv_tmu);

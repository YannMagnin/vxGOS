#include <vhex/timer/fps.h>
#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/driver/mpu/sh/sh7305/cpg.h>
#include <vhex/driver/cpu.h>

#include <string.h>

//---
// Public FPS API
//---

// TODO : proper handle
extern uint32_t *tmu_prof_tcnt;

/* sh7305_tmu_fps_init() : initialize fps object */
void sh7305_tmu_fps_init(fps_t *fps, int fps_target)
{
    sh7305_tmu_prof_init(NULL);
    memset(fps, 0x00, sizeof(fps_t));
    fps->objectif = 1000000 / fps_target;
    fps->fps_target = fps_target;
}

/* sh7305_tmu_fps_sync() : compute frame statistic and wait whanted FPS */
void sh7305_tmu_fps_sync(fps_t *fps)
{
    struct cpg_clock_frequency cpg_freq;
    uint32_t target_ticks;
    uint32_t snap;

    //TODO: proper freq calculation
    cpg_clock_freq(&cpg_freq);
    snap = (uint32_t)(((uint64_t)*tmu_prof_tcnt * 4 * 1000000) / cpg_freq.Pphi_f);

    fps->render = fps->anchor - snap;
    fps->margin = fps->objectif - fps->render;

    /* we don't secure the division because it negligent */
    fps->fps_real = 1000000 / fps->render;

    fps->fps = fps->fps_real;
    if (fps->margin > 0)
        fps->fps = fps->fps_target;

    target_ticks = snap + fps->objectif;
    while (1) {
        if (*tmu_prof_tcnt >= target_ticks)
            break;
    }

    fps->anchor = snap;
}

/* sh7305_fps_quit() : uninit the fps object */
void sh7305_tmu_fps_quit(fps_t *fps)
{
    (void)fps;
    sh7305_tmu_prof_quit(NULL);
}

//---
// drivers:screen:r61523:display    - display pipeline primitives
//---

#include "vhex/drivers/screen/r61523.h"

//---
// Public
//---

/* r61523_frame_start() - setup the display surface information
 *
 * @note
 * - we use the Casio's internal VRAM to save a lot of vram */
int r61523_frame_start(struct dsurface *surface)
{
    surface->vram   = (void*)0x8c000000;
    surface->width  = 320;
    surface->height = 528;
    surface->x1     = 0;
    surface->y1     = 0;
    surface->x2     = 319;
    surface->y2     = 527;
    surface->number = 0;
    return 0;
}

/* r61523_frame_next() : next surface fragment */
int r61523_frame_next(struct dsurface *surface)
{
    (void)surface;
    return -1;
}

/* r61523_frame_send() : send the fragment to screen */
int r61523_frame_send(struct dsurface *surface)
{
    return r61523_hw_vram_send(
        surface->x1,
        surface->y1,
        surface->x2,
        surface->y2,
        surface->vram
    );
}

/* r61523_frame_end() : "close" the surface */
int r61523_frame_end(struct dsurface *surface)
{
    (void)surface;
    return 0;
}

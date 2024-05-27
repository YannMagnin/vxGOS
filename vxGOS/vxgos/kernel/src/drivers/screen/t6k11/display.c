//---
// drivers:screen:t6k11:display    - display pipeline primitives
//---

#include "vhex/drivers/screen/t6k11.h"

//---
// Public
//---

/* t6k11_frame_start() - setup the display surface information
 *
 * @note
 * - the VRAM is defined in driver specific (t6k11.c) */
int t6k11_frame_start(struct dsurface *surface)
{
    extern void *t6k11_vram;

    surface->vram   = t6k11_vram;
    surface->width  = T6K11_SCREEN_WIDTH;
    surface->height = T6K11_SCREEN_HEIGHT;
    surface->x1     = 0;
    surface->y1     = 0;
    surface->x2     = T6K11_SCREEN_WIDTH - 1;
    surface->y2     = T6K11_SCREEN_HEIGHT - 1;
    surface->number = 0;
    return 0;
}

/* t6k11_frame_frag_next() : next surface fragment */
int t6k11_frame_frag_next(struct dsurface *surface)
{
    (void)surface;
    return -1;
}

/* t6k11_frame_frag_send() : send the fragment to screen */
int t6k11_frame_frag_send(struct dsurface *surface)
{
    return t6k11_hw_vram_send(
        surface->x1,
        surface->y1,
        surface->x2,
        surface->y2,
        surface->vram
    );
}

/* t6k11_frame_end() : "close" the surface */
int t6k11_frame_end(struct dsurface *surface)
{
    (void)surface;
    return 0;
}

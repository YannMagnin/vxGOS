//---
// driver:screen:r61524:display - display pipeline primitives
//---

#include "vhex/drivers/screen/r61524.h"
#include "vhex/modules/display/surface.h"

//---
// Internals
//---

/* create the two internal VRAM */
static uint32_t __vram0[(R61524_SCREEN_WIDTH * R61524_SCREEN_HEIGHT) / 2];
static uint32_t __vram1[(R61524_SCREEN_WIDTH * R61524_SCREEN_HEIGHT) / 2];

/* current selected vram */
static uint32_t *vram = (void*)&__vram0;

//---
// Public
//---

/* r61524_frame_start() - setup the display surface */
int r61524_frame_start(struct dsurface *surface)
{
    surface->vram   = (void*)vram;
    surface->width  = R61524_SCREEN_WIDTH;
    surface->height = R61524_SCREEN_HEIGHT;
    surface->x1     = 0;
    surface->y1     = 0;
    surface->x2     = R61524_SCREEN_WIDTH - 1;
    surface->y2     = R61524_SCREEN_HEIGHT - 1;
    return (0);
}

/* r61524_frame_frag_next() : next surface fragment */
int r61524_frame_frag_next(struct dsurface *surface)
{
    (void)surface;
    return -1;
}

/* r61523_frame_frag_send() : send the fragment to screen */
int r61524_frame_frag_send(struct dsurface *surface)
{
    return r61524_hw_vram_send(
        surface->x1,
        surface->y1,
        surface->x2,
        surface->y2,
        surface->vram
    );
}

/* r61524_frame_end() : "close" the surface */
int r61524_frame_end(struct dsurface *surface)
{
    (void)surface;
    return (0);
}

#ifndef VHEX_DRIVERS_SCREEN_R61524_H
#define VHEX_DRIVERS_SCREEN_R61524_H 1

#include <stdint.h>

#include "vhex/modules/display/surface.h"

//---
// Public
//---


// driver primitives

#define R61524_SCREEN_WIDTH     396
#define R61524_SCREEN_HEIGHT    224

/* driver context */
struct r61524_drv_ctx
{
    uint16_t HSA;
    uint16_t HEA;
    uint16_t VSA;
    uint16_t VEA;
};

/* r61524_hw_configure() : prepare the driver context */
extern void r61524_hw_configure(struct r61524_drv_ctx *context);

/* r61524_hw_hsave() : save the current driver context */
extern void r61524_hw_hsave(struct r61524_drv_ctx *context);

/* r61524_hw_hrestore() : restore saved context */
extern void r61524_hw_hrestore(struct r61524_drv_ctx const *context);


// display primitives


/* r61524_frame_start() - setup the display surface */
extern int r61524_frame_start(struct dsurface *surface);

/* r61524_frame_frag_next() : next surface fragment */
extern int r61524_frame_frag_next(struct dsurface *surface);

/* r61523_frame_frag_send() : send the fragment to screen */
extern int r61524_frame_frag_send(struct dsurface *surface);

/* r61524_frame_end() : "close" the surface */
extern int r61524_frame_end(struct dsurface *surface);


// "on-screen" primitives


/* r61524_hw_dclear() : optimised screen clearing without vram */
extern int r61524_hw_dclear(int color);

/* r61524_hw_dpixel() : drawing pixel on screen */
extern int r61524_hw_dpixel(int x, int y, int color);

/* r61524_hw_dscroll() : scroll the screen */
extern int r61524_hw_dscroll(int offset, int direction);


// low-level primitives


/* r61524_hw_vram_send() : send the vram to the screen */
extern int r61524_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

/* r61524_hw_vram_fetch() : fetch "on-screen" pixels */
extern int r61524_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

#endif /* VHEX_DRIVER_SCREEN_R61524_H */

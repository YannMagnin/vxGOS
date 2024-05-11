#ifndef VHEX_DRIVERS_SCREEN_R61523_H
#define VHEX_DRIVERS_SCREEN_R61523_H 1

#include <stdint.h>

#include "vhex/modules/display/surface.h"

//---
// Public
//---

/* driver context */
// todo
struct r61523_drv_ctx
{
    int magic;
};


// display primitives


/* r61523_frame_start() - setup the (physical) screen and reset surface */
extern int r61523_frame_start(struct dsurface *surface);

/* r61523_frame_next() : next surface fragment */
extern int r61523_frame_next(struct dsurface *surface);

/* r61523_frame_send() : send the fragment to screen */
extern int r61523_frame_send(struct dsurface *surface);

/* r61523_frame_end() : "close" the surface */
extern int r61523_frame_end(struct dsurface *surface);


// "on-screen" drawing primitive


/* r61523_hw_dclear() : optimised screen clearing without vram */
extern int r61523_hw_dclear(int color);

/* r61523_hw_dpixel() : drawing pixel on screen */
extern int r61523_hw_dpixel(int x, int y, int color);

/* r61523_hw_dscroll() : scroll the screen */
extern int r61523_hw_dscroll(int offset, int direction);


// low-level primitives


/* r61523_hw_vram_send() : send the vram to the screen */
extern int r61523_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

/* r61523_hw_vram_fetch() : fetch "on-screen" pixels */
extern int r61523_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

#endif /* VHEX_DRIVERS_SCREEN_R61523_H */

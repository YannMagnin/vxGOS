
#ifndef VHEX_DRIVERS_SCREEN_T6K11_H
#define VHEX_DRIVERS_SCREEN_T6K11_H 1

#include <stdint.h>

#include "vhex/modules/display/surface.h"

//---
// Internals (should not be used)
//---

// T6K73 primitives

/* t6k73_hw_dclear() : clear screen with color */
extern int t6k73_hw_dclear(int color);

/* t6k73_hw_dpixel() : drawing pixel on screen */
extern int t6k73_hw_dpixel(int x, int y, int color);

/* t6k73_hw_vram_send() : send the vram to the screen */
extern int t6k73_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

/* t6k73_hw_vram_fetch() : fetch "on-screen" pixels */
extern int t6k73_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

// ML9801 primitives

/* ml9801_hw_dclear() : clear screen with color */
extern int ml9801_hw_dclear(int color);

/* ml9801_hw_dpixel() : drawing pixel on screen */
extern int ml9801_hw_dpixel(int x, int y, int color);

/* ml9801_hw_vram_send() : send the vram to the screen */
extern int ml9801_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

/* ml9801_hw_vram_fetch() : fetch "on-screen" pixels */
extern int ml9801_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

//---
// Public
//---

// driver primitives

#define T6K11_SCREEN_WIDTH     128
#define T6K11_SCREEN_HEIGHT    64

/* driver context */
struct t6k11_drv_ctx
{
    int magic;
};

/* t6k11_hw_configure() : prepare the driver context */
extern void t6k11_hw_configure(struct t6k11_drv_ctx *context);


// display primitives


/* t6k11_frame_start() - setup the display surface */
extern int t6k11_frame_start(struct dsurface *surface);

/* t6k11_frame_frag_next() : next surface fragment */
extern int t6k11_frame_frag_next(struct dsurface *surface);

/* t6k11_frame_frag_send() : send the fragment to screen */
extern int t6k11_frame_frag_send(struct dsurface *surface);

/* t6k11_frame_end() : "close" the surface */
extern int t6k11_frame_end(struct dsurface *surface);


// "on-screen" primitives


/* t6k11_hw_dclear() : optimised screen clearing without vram */
extern int t6k11_hw_dclear(int color);

/* t6k11_hw_dpixel() : drawing pixel on screen */
extern int t6k11_hw_dpixel(int x, int y, int color);

/* t6k11_hw_dscroll() : scroll the screen */
extern int t6k11_hw_dscroll(int offset, int direction);


// low-level primitives


/* t6k11_hw_vram_send() : send the vram to the screen */
extern int t6k11_hw_vram_send(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

/* t6k11_hw_vram_fetch() : fetch "on-screen" pixels */
extern int t6k11_hw_vram_fetch(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    uint16_t *vram
);

#endif /* VHEX_DRIVERS_SCREEN_T6K11_H */

#ifndef VHEX_MODULES_DISPLAY_DRAW_CIRCLE_H
#define VHEX_MODULES_DISPLAY_DRAW_CIRCLE_H 1

#include <stdint.h>
#include <stddef.h>

//---
// Public
//---

enum {
    /* Horizontal settings: default in dcircle*() is DCIRCLE_LEFT */
    DCIRCLE_LEFT   = 0x01,
    DCIRCLE_CENTER = 0x02,
    DCIRCLE_RIGHT  = 0x04,
    /* Vertical settings: default in dcircle() is DCIRCLE_TOP */
    DCIRCLE_TOP    = 0x10,
    DCIRCLE_MIDDLE = 0x20,
    DCIRCLE_BOTTOM = 0x40,
};

/* dcircle_filled() : draw a filled circle */
extern void dcircle_filled(int x, int y, size_t radius, int mode, int color);

#endif /* VHEX_DISPLAY_DRAW_CIRCLE_H */

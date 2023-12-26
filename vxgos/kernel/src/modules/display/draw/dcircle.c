#include <vhex/display/draw/circle.h>
#include <vhex/display/draw/line.h>
#include <vhex/display.h>

//---
// Kernel-level API
//---

/* dcircle_render() : real drawing algorithm */
void dcircle_filled_render(
    dsurface_t *surface,
    int x, int y,
    int radius,
    int color
) {
    int px;
    int py;
    int d;

    px = 0;
    py = radius;
    d = 1 - radius;

    dhline_render(surface, y, x - py, x + py, color);
    while(py > px)
    {
        if(d < 0) {
            d += (2 * px) + 3;
        } else {
            d += (2 * (px - py)) + 5;
            py--;
            dhline_render(surface, y + py + 1, x - px, x + px, color);
            dhline_render(surface, y - py - 1, x - px, x + px, color);
        }
        px++;
        if(py >= px) {
            dhline_render(surface, y + px, x - py, x + py, color);
            dhline_render(surface, y - px, x - py, x + py, color);
        }
    }
}

//---
// Dstack-level API
//---

/* dcircle_filled_dstack() : dstack wrapper primitive */
void dcircle_filled_dstack(dsurface_t *surface, uintptr_t *args)
{
    dcircle_filled_render(
        surface,
        (int)args[0],
        (int)args[1],
        (size_t)args[2],
        (int)args[3]
    );
}

//---
// User-level API
//---

/* dcircle_filled() : draw a filled circle */
void dcircle_filled(int x, int y, size_t radius, int mode, int color)
{
    if (color == C_NONE) return;

    int offset = radius & 1;

    if (mode & DCIRCLE_LEFT)   x += (radius >> 1) + offset;
    if (mode & DCIRCLE_RIGHT)  x -= (radius >> 1) + offset;
    if (mode & DCIRCLE_TOP)    y += (radius >> 1) + offset;
    if (mode & DCIRCLE_BOTTOM) y -= (radius >> 1) + offset;

    dstack_add_action(
        DSTACK_CALL(&dcircle_filled_dstack, x, y, radius, color),
        NULL,
        NULL
    );
}

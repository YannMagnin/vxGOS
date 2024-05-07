//---
// modules:display:draw:dline   - display line
//---

#include "vhex/modules/display/draw/line.h"
#include "vhex/modules/display/surface.h"
#include "vhex/modules/display/color.h"
#include "vhex/modules/display/stack.h"
#include "vhex/defs/utils.h"

//---
// Internals
//---

/* external symbols */
extern void dpixel_render(struct dsurface *surface, int x, int y, int color);

/* dline(): Bresenham line drawing algorithm
 *
 * @note
 * - public symbol to allow invocation in other render primitives
 * - hardcoded for 16-bit color
 * - remotely adapted from MonochromeLib code by Pierre "PerriotLL" Le Gall.
 * - relies on platform-dependent dhline() and dvline() for optimized
 *   situations */
void dline_render(
    struct dsurface *surface,
    int color,
    int x1,
    int y1,
    int x2,
    int y2
) {
    int i;
    int x = x1;
    int y = y1;
    int cumul;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int sx = sgn(dx);
    int sy = sgn(dy);

    dx = (dx >= 0 ? dx : -dx);
    dy = (dy >= 0 ? dy : -dy);

    dpixel_render(surface, x1, y1, color);

    if(dx >= dy) {
        /* Start with a non-zero cumul to even the overdue between the
           two ends of the line (for more regularity) */
        cumul = dx >> 1;
        for(i = 1; i < dx; i++)
        {
            x += sx;
            cumul += dy;
            if(cumul > dx) {
                cumul -= dx;
                y += sy;
            }
            dpixel_render(surface, x, y, color);
        }
    } else {
        cumul = dy >> 1;
        for(i = 1; i < dy; i++)
        {
            y += sy;
            cumul += dx;
            if(cumul > dy) {
                cumul -= dy;
                x += sx;
            }
            dpixel_render(surface, x, y, color);
        }
    }

    dpixel_render(surface, x2, y2, color);
}

/* dhline_render() : optimized horizontal line
 *
 * @note
 * - hardcoded for 16-bit color */
void dhline_render(
    struct dsurface *surface,
    int color,
    int y,
    int x1,
    int x2
) {
    /* Order and bounds */
    if(y  < surface->y1 || y  > surface->y2) return;
    if(x1 > surface->x2 || x2 < surface->x1) return;
    if(x1 > x2) swap(x1, x2);
    if(x1 < surface->x1) x1 = surface->x1;
    if(x2 > surface->x2) x2 = surface->x2;
    y  -= surface->y1;
    x1 -= surface->x1;
    x2 -= surface->x1;

    int offset = surface->width * y;

    /* Use longwords to do the copy, but first paint the endpoints to heed
       for odd x1 and x2. Checking the parity may be a waste of time. */
    uint16_t *vram = surface->vram;
    if (color != C_INVERT) {
        vram[offset + x1] = color;
        vram[offset + x2] = color;
    } else {
        vram[offset + x1] ^= 0xffff;
        vram[offset + x2] ^= 0xffff;
    }

    /* Now round to longword boundaries and copy everything in-between with
       longwords */
    x1 = x1 + (x1 & 1);
    x2 = (x2 + 1) & ~1;

    uint32_t *start = (void *)(vram + offset + x1);
    uint32_t *end   = (void *)(vram + offset + x2);
    if (color != C_INVERT) {
        uint32_t op = (color << 16) | color;
        while(end > start) {
            *--end = op;
        }
    } else {
        while(end > start) {
            *--end ^= 0xffff;
        }
    }
}

/* dvline_render() : optimized drawing function for vertical line
 *
 * @note
 * - hardcoded for 16-bit color */
void dvline_render(
    struct dsurface *surface,
    int color,
    int x,
    int y1,
    int y2
) {
    /* Order and bounds */
    if(x < surface->x1 || x > surface->x2)   return;
    if(y1 > surface->y2 || y2 < surface->y1) return;
    if(y1 > y2) swap(y1, y2);
    if(y1 < surface->y1) y1 = surface->y1;
    if(y2 > surface->y2) y2 = surface->y2;
    x  -= surface->x1;
    y1 -= surface->y1;
    y2 -= surface->y1;

    uint16_t *vram = surface->vram;
    uint16_t *v = &vram[(surface->width * y1) + x];
    int height = y2 - y1;

    if (color != C_INVERT) {
        while(height-- >= 0) {
            *v = color;
            v += surface->width;
        }
    } else {
        while(height-- >= 0) {
            *v ^= 0xffff;
            v += surface->width;
        }
    }
}

// display stack

/* dline_dstack() : dstack wrapper line primitive */
static void dline_dstack(struct dsurface *surface, uintptr_t *args)
{
    dline_render(
        surface,
        (int)args[0],
        (int)args[1],
        (int)args[2],
        (int)args[3],
        (int)args[4]
    );
}

/* dhline_dstack() : dstack wrapper horizontal line primitive */
static void dhline_dstack(struct dsurface *surface, uintptr_t *args)
{
    dhline_render(
        surface,
        (int)args[0],
        (int)args[1],
        (int)args[2],
        (int)args[3]
    );
}

/* dvline_dstack() : dstack wrapper vertical line primitive */
static void dvline_dstack(struct dsurface *surface, uintptr_t *args)
{
    dvline_render(
        surface,
        (int)args[0],
        (int)args[1],
        (int)args[2],
        (int)args[3]
    );
}

//---
// Public
//---

/* dline(): Render a straight line
 *
 * @note
 * - support optimized drawing for horizontal and vertical line */
void dline(int color, int x1, int y1, int x2, int y2)
{
    if(color == C_NONE)
        return;
    if(y1 == y2)
    {
        dstack_add_action(
            DSTACK_CALL(&dhline_dstack, color, y1, x1, x2),
            NULL,
            NULL
        );
        return;
    }
    if(x1 == x2)
    {
        dstack_add_action(
            DSTACK_CALL(&dvline_dstack, color, x1, y1, y2),
            NULL,
            NULL
        );
        return;
    }
    dstack_add_action(
        DSTACK_CALL(&dline_dstack, color, x1, y1, x2, y2),
        NULL,
        NULL
    );
}

/* dhline() : render an horizontal line */
void dhline(int color, int y, int x1, int x2)
{
    dstack_add_action(
        DSTACK_CALL(&dhline_dstack, color, y, x1, x2),
        NULL,
        NULL
    );
}

/* dvline() : render an vertical line */
void dvline(int color, int x, int y1, int y2)
{
    dstack_add_action(
        DSTACK_CALL(&dvline_dstack, color, x, y1, y2),
        NULL,
        NULL
    );
}

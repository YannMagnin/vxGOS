//---
// modules:display:draw:pixel   - display pixel
//---

#include "vhex/modules/display/surface.h"
#include "vhex/modules/display/surface.h"
#include "vhex/modules/display/stack.h"
#include "vhex/modules/display/color.h"

//---
// Internal
//---

/* dpixel_render() : drawing algorithm */
void dpixel_render(struct dsurface *surface, int x, int y, int color)
{
    if (color == C_NONE)
        return;

    /* check point culling */
    if (y < surface->y1
        || x < surface->x1
        || y > surface->y2
        || x > surface->x2) {
        return;
    }

    /* calculate the draw index */
    int real_y   = (y - surface->y1) * surface->width;
    int real_x   = (x - surface->x1);
    int draw_idx = real_y + real_x;

    /* handle special color */
    uint16_t *vram = surface->vram;
    if (color == C_INVERT)
        color = vram[draw_idx] ^ 0xffff;

    /* set the pixel */
    vram[draw_idx] = color;
}

/* dpixel_render_dstack() : dstack-API compatible render */
static void dpixel_dstack(struct dsurface *surface, uintptr_t *arg)
{
    dpixel_render(surface, arg[0], arg[1], arg[2]);
}

//---
// Public
//---

/* dpixel() : draw a pixel in screen */
void dpixel(int x, int y, int color)
{
    dstack_add_action(
        DSTACK_CALL(&dpixel_dstack, x, y, color),
        NULL,
        NULL
    );
}

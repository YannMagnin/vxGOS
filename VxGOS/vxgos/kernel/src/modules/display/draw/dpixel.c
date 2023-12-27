#include <vhex/display.h>
#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

//---
// Kernel-level API
//---

/* dpixel_render() : drawing algorithm */
void dpixel_render(dsurface_t *surface, int x, int y, int color)
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

//---
// Dstack-level API
//---

/* dpixel_render_dstack() : dstack-API compatible render */
void dpixel_dstack(dsurface_t *surface, uintptr_t *arg)
{
    dpixel_render(surface, arg[0], arg[1], arg[2]);
}

//---
// User-level API
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


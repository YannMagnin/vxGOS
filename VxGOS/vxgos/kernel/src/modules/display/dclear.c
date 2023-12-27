#include <vhex/defs/types.h>
#include <vhex/display.h>
#include <vhex/display/stack.h>

//---
// kernel-level API
//---

/* dclear_draw() : real drawing algorithm */
void dclear_render(dsurface_t *surface, uint32_t color)
{
    uint32_t *vram = surface->vram;
    for (size_t i = 0; i < surface->width * (surface->height/2); ++i)
        vram[i] = color;
#if 0
    uint32_t *vram;
    int size = (surface->y1 == 220) ? 792 : 1980;

    vram = surface->vram;
    for (int i = 0; i < size; ++i)
        vram[i] = color;
#endif
}

//---
// Dstack-level API
//---

/* dclear_dstack() : dstack rwrapper primitive */
void dclear_dstack(dsurface_t *surface, uintptr_t *arg)
{
    dclear_render(surface, (uint32_t)arg[0]);
}

//---
// User-level API
//---

/* dclear(): Fill the screen with a single color */
did_t dclear(int color)
{
    uint32_t copti;

    dstack_invalidate();

    if (color == C_NONE)
        return 0;

    color = color & 0xffff;
    copti = (color << 16) | (color << 0);
    return dstack_add_action(
        DSTACK_CALL(&dclear_dstack, copti),
        NULL,
        NULL
    );
}

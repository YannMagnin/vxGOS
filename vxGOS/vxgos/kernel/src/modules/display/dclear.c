//---
// modules:display:dclear   - display clear render operation
//---

#include <stdint.h>

#include "vhex/modules/display.h"
#include "vhex/modules/display/stack.h"
#include "vhex/modules/display/surface.h"

//---
// Internals
//---

/* dclear_render() : real drawing algorithm
 *
 * @note
 * - low-level drawing invoked by dstack_render()
 * - should never be called manually */
static void dclear_render(struct dsurface *surface, uint32_t color)
{
    uint32_t *vram = surface->vram;
    for (size_t i = 0; i < surface->width * (surface->height/2); ++i)
        vram[i] = color;
}

/* dclear_dstack() : dstack wrapper primitive
 *
 * @note
 * - deserialise dstack invokation */
static void dclear_dstack(struct dsurface *surface, uintptr_t *arg)
{
    dclear_render(surface, (uint32_t)arg[0]);
}

//---
// Public
//---

/* dclear(): Fill the screen with a single color
 *
 * @note
 * - currently hardcoded for 16-bits color */
int dclear(int color)
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

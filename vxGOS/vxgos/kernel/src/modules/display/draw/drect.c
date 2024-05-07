//---
// modules:display:draw:drect   - display rectangle render
//---

#include "vhex/modules/display/draw/rect.h"
#include "vhex/modules/display/surface.h"
#include "vhex/modules/display/stack.h"
#include "vhex/defs/utils.h"

//---
// Internals
//---

/*external symbols */
extern void dhline_render(
    struct dsurface *surface,
    int color,
    int y,
    int x1,
    int x2
);

/* drect_filled_render() : draw filled rectangle */
void drect_filled_render(
    struct dsurface *surface,
    int color,
    int x1,
    int y1,
    int x2,
    int y2
) {
    if (y1 > y2)
        swap(y1, y2);
    for (int y = y1 ; y <= y2 ; ++y) {
        dhline_render(surface, color, y, x1, x2);
    }
}

/* drect_filled_dstack() : dstack API wrapper */
static void drect_filled_dstack(struct dsurface *surface, uintptr_t *args)
{
    drect_filled_render(
        surface,
        (int)args[0],
        (int)args[1],
        (int)args[2],
        (int)args[3],
        (int)args[4]
    );
}

//---
// Public
//---

/* drect() : draw a rectangle */
void drect(int color, int x1, int y1, int x2, int y2)
{
    dstack_add_action(
        DSTACK_CALL(&drect_filled_dstack, color, x1, y1, x2, y2),
        NULL,
        NULL
    );
}

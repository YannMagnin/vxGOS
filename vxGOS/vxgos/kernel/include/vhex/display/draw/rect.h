#ifndef __VHEX_DISPLAY_DRAW_RECT__
# define __VHEX_DISPLAY_DRAW_RECT__

//---
// User-level API
//---

/* drect_filled() : draw filled rectangle */
extern void drect(int color, int x1, int y1, int x2, int y2);

//---
// Kernel-level API
//---

#include <vhex/display/types.h>

/* drect_filled() : draw filled rectangle */
extern void drect_filled_render(
    dsurface_t *surface,
    int color,
    int x1, int y1, int x2, int y2
);

#endif /* __VHEX_DISPLAY_DRAW_RECT__ */

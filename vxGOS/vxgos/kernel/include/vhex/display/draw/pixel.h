#ifndef __VHEX_DISPLAY_DRAW_PIXEL__
# define __VHEX_DISPLAY_DRAW_PIXEL__

#include <vhex/display/types.h>
#include <vhex/display/shader.h>

//---
// User-level API
//---

/* dpixel() : draw a pixel in screen */
extern void dpixel(int x, int y, int color);

//---
// Kernel-level API
//---

/* dpixel_render() : drawing algorithm */
extern void dpixel_render(dsurface_t *surface, int x, int y, int c);

#endif /* __VHEX_DISPLAY_DRAW_PIXEL__ */

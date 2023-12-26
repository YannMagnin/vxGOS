#ifndef __VHEX_DISPLAY_FONT_RENDER__
# define __VHEX_DISPLAY_FONT_RENDER__

#include <vhex/display/types.h>

//---
// User-level API
//---

/* dfont_cjar() : display one char */
extern void dfont_char(uint32_t n, int x, int y, int fg, int bg);

//---
// kernel-level API
//---

extern void dfont_char_render(
    dsurface_t *surface,
    int glyph_idx,
    uintptr_t *arg
);

#endif /* __VHEX_DISPLAY_FONT_RENDER__ */

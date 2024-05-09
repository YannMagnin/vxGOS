//---
// fxcp400:kterm:kterm  - board specific kterm
//---

#include "vhex/devices/kterm.h"

//---
// Public
//---

/* kterm_font_get() : get the default kterm font */
int kterm_font_get(struct vxfont **font)
{
    extern struct vxfont font8x9;

    if (font == NULL)
        return -1;
    *font = &font8x9;
    return 0;
}

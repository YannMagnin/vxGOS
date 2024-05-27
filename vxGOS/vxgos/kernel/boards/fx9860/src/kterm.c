//---
// fx9860:kterm:kterm   - board specific kterm
//---

#include "vhex/devices/kterm.h"

//---
// Public
//---

/* kterm_font_get() : get the default kterm font */
int kterm_font_get(struct vxfont **font)
{
    extern struct vxfont font3x5;

    if (font == NULL)
        return -1;
    *font = &font3x5;
    return 0;
}

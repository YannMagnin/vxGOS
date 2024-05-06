//---
// devices:kterm:clear  - clear screen and reset cursor information
//---

#include "vhex/devices/kterm.h"

//---
// Internals
//---

/* the kterm device defined in `kterm_initialise()` */
extern struct _kterm _kterm;

/* the low-level clear drawing primitive */
extern void dclear_render(struct dsurface *surface, uint32_t color);

//---
// Public
//---

/* kterm_clear() : clear screen and reset cursor position */
int kterm_clear(int color)
{
    if (_kterm.__magic != _KTERM_INIT_MAGIC)
        return -1;
    _kterm.cursor.x = 0;
    _kterm.cursor.y = 0;
    _kterm.flags.NEED_SCROLL = 0;
    _kterm.display.driver.bios.dclear(color);
    return 0;
}

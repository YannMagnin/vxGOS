#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* _bios_dfont_get() : get font information */
int _bios_dfont_get(struct font **font)
{
    extern struct font font3x5;

    if (font == NULL)
        return -1;
    *font = &font3x5;
    return 0;
}
